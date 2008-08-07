#include <config.h>
#include <gtk/gtk.h>

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_printerr("<GnomenuGlobalMenu>::" fmt "\n",  ## args)
#else
#define LOG(fmt, args...)
#endif
#define LOG_FUNC_NAME LOG("%s", __func__)

#include <gdk/gdkx.h>
#include "ipc.h"
GdkNativeWindow ipc_find_server() {
	GList * window_list = NULL;
	GdkNativeWindow rt = 0;
	GdkWindow * root_window;
    Window root_return;
    Window parent_return;
    Window * children_return;
	Display * display = GDK_DISPLAY_XDISPLAY(gdk_display_get_default());
	gchar * name = IPC_SERVER_TITLE;

    unsigned int nchildren_return;
    unsigned int i;

	root_window = gdk_get_default_root_window();
	g_return_val_if_fail(root_window != NULL, NULL);

    gdk_error_trap_push();
    XQueryTree(display,
        GDK_WINDOW_XWINDOW(root_window),
        &root_return,
        &parent_return,
        &children_return,
        &nchildren_return);
	gdk_flush();
    if(gdk_error_trap_pop()){
        g_warning("%s: XQueryTree Failed", __func__);
        return rt;
    }
	g_return_val_if_fail(nchildren_return != 0, NULL);
		
	for(i = 0; i < nchildren_return; i++){
        Atom type_return;
        Atom type_req = XInternAtom (display, "UTF8_STRING", FALSE);
        gint format_return;
        gulong nitems_return;
        gulong bytes_after_return;
        guchar * wm_name;
        gint rt;
        gdk_error_trap_push();
        rt = XGetWindowProperty (display, children_return[i],
                          XInternAtom (display, "_NET_WM_NAME", FALSE),
                          0, G_MAXLONG, False, type_req, &type_return,
                          &format_return, &nitems_return, &bytes_after_return,
                          &wm_name);
		gdk_flush();
		if(!gdk_error_trap_pop()){
			if(rt == Success && type_return == type_req){
				if(name == NULL || g_str_equal(name, wm_name)){
					window_list = g_list_append(window_list, (gpointer) children_return[i]);
				}
			XFree(wm_name);
			}
		}else{
			g_warning("%s:XGetWindowProperty Failed",__func__);
		}
	}
	XFree(children_return);
	if(window_list) {
		rt = window_list->data;
		g_list_free(window_list);
	}
	return rt;
}
gpointer ipc_wait_for_property(GdkNativeWindow window, GdkAtom property_name, gboolean remove){
	Display * display = GDK_DISPLAY_XDISPLAY(gdk_display_get_default());
	Atom type_return;
	unsigned long format_return;
	unsigned long remaining_bytes;
	unsigned long nitems_return;
	gpointer data = NULL;
	gint i = 0;
	while(i<100) {
		gdk_error_trap_push();
		XGetWindowProperty(display,
				window,
				gdk_x11_atom_to_xatom(property_name),
				0,
				-1,
				remove,
				AnyPropertyType,
				&type_return,
				&format_return,
				&nitems_return,
				&remaining_bytes,
				&data);
		if(gdk_error_trap_pop()) {
			return NULL;
		} else {
			if(type_return != None)
				return data;
		}
		i++;
		g_usleep(i* 1000);
	}
	return data;
}
void ipc_send_client_message(GdkNativeWindow from, GdkNativeWindow to, GdkAtom message_type) {
	GdkEventClient ec;
	ec.type = GDK_CLIENT_EVENT;
	ec.window = 0;
	ec.send_event = TRUE;
	ec.message_type = message_type;
	ec.data_format = 8;
	*((GdkNativeWindow *)&ec.data.l[0]) = from;
	gdk_event_send_client_message(&ec, to);
}
void ipc_set_property(GdkNativeWindow  window, GdkAtom property, gchar * string){
	Display * display = GDK_DISPLAY_XDISPLAY(gdk_display_get_default());
	XChangeProperty(display,
			window,
			gdk_x11_atom_to_xatom(property),
			gdk_x11_atom_to_xatom(property), /*type*/
			8,
			PropModeReplace,
			string,
			strlen(string) + 1);
	XSync(display, FALSE);
}
gchar * ipc_get_property(GdkNativeWindow src, GdkAtom property_name){
	Display * display = GDK_DISPLAY_XDISPLAY(gdk_display_get_default()) ;
	guchar * data;
	Atom type_return;
	guint format_return;
	unsigned long nitems_return;
	unsigned long remaining_bytes;
	gdk_error_trap_push();
	XGetWindowProperty(display,
			src,
			gdk_x11_atom_to_xatom(property_name),
			0,
			-1,
			TRUE,
			AnyPropertyType,
			&type_return,
			&format_return,
			&nitems_return,
			&remaining_bytes,
			&data);
	if(gdk_error_trap_pop()){
		return NULL;
	} else {
		if(type_return == None) return NULL;
		return data;
	}
}
