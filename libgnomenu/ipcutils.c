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
