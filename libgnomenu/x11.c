
#include <config.h>
#include "x11.h"


#ifndef GDK_WINDOWING_X11
#error ONLY X11 is supported. other targets are not yet supported.
#endif

#if ENABLE_TRACING >= 3
#define LOG(fmt, args...) g_message("<GnomenuSocket>::%s:" fmt, __func__, ## args)
#else
#define LOG(fmt, args...)
#endif 


gboolean _send_xclient_message 	( GdkNativeWindow target, gpointer data, guint bytes ){
	gboolean rt;
    XClientMessageEvent xclient;
	if(bytes > 20){
		g_error("GnomenuSocket: Can not send raw data for more than 20 bytes");
		return FALSE; /*X can not send more information*/
	}

    memset (&xclient, 0, sizeof (xclient));
    xclient.window = target; /*Though X11 places no interpretation of this field, GNOMENU interpretes this field at the target window.*/
    xclient.type = ClientMessage;
    xclient.message_type = gdk_x11_atom_to_xatom(gdk_atom_intern(_GNOMENU_MESSAGE_TYPE, FALSE));
    xclient.format = 8;
	memcpy(&xclient.data.l, data, bytes);
    gdk_error_trap_push ();
    XSendEvent (GDK_DISPLAY_XDISPLAY(gdk_display_get_default()),
          target,
          False, NoEventMask, (XEvent *)&xclient);
	gdk_flush();
    gdk_display_sync (gdk_display_get_default()); /*resolve the message, sync the state*/
    return gdk_error_trap_pop () == 0;

}

gboolean _peek_xwindow 			( GdkNativeWindow target ) {
	GdkNativeWindow root_return;
	GdkNativeWindow parent_return;
	GdkNativeWindow * children_return;
	guint 	nchildren_return;
	gdk_error_trap_push();
	XQueryTree(GDK_DISPLAY_XDISPLAY(gdk_display_get_default()),
			target, 
			&root_return, &parent_return, 
			&children_return, &nchildren_return);
	if(gdk_error_trap_pop()) return FALSE;
	else {
		XFree(children_return);
	}
	return TRUE;
}
/**
 * _find_native_by_name:
 * 	
 * If name == NULL, return all windows.
 */
GList * _find_native_by_name		( gchar * name ){
	GList * window_list = NULL;
	GdkScreen * screen;
	GdkWindow * root_window;

    Window root_return;
    Window parent_return;
    Window * children_return;
    unsigned int nchildren_return;
    unsigned int i;
	gchar * gnomenu_type;
	screen = gdk_screen_get_default();
	g_return_val_if_fail(screen, NULL);

	root_window = gdk_screen_get_root_window(screen);
	g_return_val_if_fail(root_window, NULL);

    gdk_error_trap_push();
    XQueryTree(GDK_DISPLAY_XDISPLAY(gdk_display_get_default()),
        GDK_WINDOW_XWINDOW(root_window),
        &root_return,
        &parent_return,
        &children_return,
        &nchildren_return);
	gdk_flush();
    if(gdk_error_trap_pop()){
        g_warning("%s: XQueryTree Failed", __func__);
        return NULL;
    }
	g_return_val_if_fail(nchildren_return != 0, NULL);
		
	for(i = 0; i < nchildren_return; i++){
        Atom type_return;
        Atom type_req = gdk_x11_atom_to_xatom (gdk_atom_intern("UTF8_STRING", FALSE));
        gint format_return;
        gulong nitems_return;
        gulong bytes_after_return;
        guchar * wm_name;
        gint rt;
        gdk_error_trap_push();
        rt = XGetWindowProperty (GDK_DISPLAY_XDISPLAY (gdk_display_get_default()), children_return[i],
                          gdk_x11_atom_to_xatom (gdk_atom_intern("_NET_WM_NAME", FALSE)),
                          0, G_MAXLONG, False, type_req, &type_return,
                          &format_return, &nitems_return, &bytes_after_return,
                          &wm_name);
		gdk_flush();
		if(!gdk_error_trap_pop()){
			if(rt == Success && type_return == type_req){
			if(g_str_equal(name, wm_name)){
				window_list = g_list_append(window_list, (gpointer) children_return[i]);
			}
			XFree(wm_name);
		}
		}else{
			g_warning("%s:XGetWindowProperty Failed",__func__);
		}
	}
	XFree(children_return);
	return window_list;

}
gchar * x_prop_name_encode(const char * name, guint id){
	gchar * full_name = g_strdup_printf("%s:%d", name, id);
	return full_name;
}
gboolean x_prop_name_decode(const char * xpropname, gchar ** name, guint * id){
	gchar ** splitted = g_strsplit(xpropname, ":", 0);
	if(g_strv_length(splitted) !=2) {
		g_strfreev(splitted);
		*name = NULL;
		return FALSE;
	}
	if(name) *name = g_strdup(splitted[0]);
	if(id) *id = (guint) g_ascii_strtoull(splitted[1], NULL, 10);
	g_strfreev(splitted);
	return TRUE;
}
gboolean _set_native_buffer ( GdkNativeWindow native, const gchar *  buffer_name, guint id, gpointer data, gint bytes){
	gchar * full_name = x_prop_name_encode(buffer_name, id);
	gdk_error_trap_push();	
	XChangeProperty(
			GDK_DISPLAY_XDISPLAY(gdk_display_get_default()), 
			native,
			gdk_x11_atom_to_xatom(gdk_atom_intern(full_name, FALSE)), 
			gdk_x11_atom_to_xatom(gdk_atom_intern(_GNOMENU_MESSAGE_TYPE, FALSE)), 
			8, 
			PropModeReplace, 
			data, bytes);
	g_free(full_name);
	if(gdk_error_trap_pop()){
		return FALSE;
	}
	return TRUE;
}
gpointer _get_native_buffer ( GdkNativeWindow native, const gchar * buffer_name, guint id, gint * bytes, gboolean remove){
	gchar * full_name = x_prop_name_encode(buffer_name, id);
	Atom actual_type_return;
	gulong actual_format_return;
	gulong bytes_after_return;
	gulong nitems_return;
	gchar * property_return;	

	gdk_error_trap_push();
	XGetWindowProperty(
			GDK_DISPLAY_XDISPLAY(gdk_display_get_default()), 
			native, 
			gdk_x11_atom_to_xatom(gdk_atom_intern(full_name, FALSE)), 
/*FIXME: max size is 2048 bytes*/
			0, 2048, 
			remove,
			gdk_x11_atom_to_xatom(gdk_atom_intern(_GNOMENU_MESSAGE_TYPE, FALSE)), 
			&actual_type_return,
			&actual_format_return,
			&nitems_return,
			&bytes_after_return,
			&property_return);
	g_free(full_name);
	if(gdk_error_trap_pop()){
		return NULL;
	} else {
		if(bytes) *bytes = nitems_return;
		gpointer data = g_memdup(property_return, nitems_return);
		LOG("%10s", data);
		XFree(property_return);		
		return data;	
	}
}
