#include <config.h>
#include <glib-2.0/glib.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>


/**
 * _gdkx_tools_set_window_prop_blocked:
 * 
 * blocked operation, return only if the property is set;
 */
typedef struct _GdkXToolsFilterData{
	GdkAtom prop_name;
	GdkWindow * window;
	GMainLoop * loop;
	int state;
} GdkXToolsFilterData;

GdkFilterReturn _gdkx_tools_filter(GdkXEvent * gdkxevent, GdkEvent * event, GdkXToolsFilterData * data){
	XEvent * xevent = gdkxevent;
	switch(xevent->type){
		case PropertyNotify:
		if(xevent->xproperty.window == GDK_WINDOW_XWINDOW(data->window) 
		&& xevent->xproperty.state == data->state 
		&& xevent->xproperty.atom == gdk_x11_atom_to_xatom(data->prop_name))
			g_main_loop_quit(data->loop);
	}
	return GDK_FILTER_CONTINUE;
}
gboolean _gdkx_tools_set_window_prop_blocked(GdkWindow * window, GdkAtom prop_name, gchar * buffer, gint size){
	GdkXToolsFilterData * filter_data = g_new0(GdkXToolsFilterData,1);
	gboolean rt = TRUE;
	Display *display = GDK_DISPLAY_XDISPLAY(gdk_drawable_get_display(window));
	Window w = GDK_WINDOW_XWINDOW(window);
	Atom property = gdk_x11_atom_to_xatom(prop_name);
   	Atom type = gdk_x11_atom_to_xatom(prop_name);
	int format = 8;
	int mode = PropModeReplace;
	unsigned char *data = buffer;
	int nelements = size;
	filter_data->loop = g_main_loop_new(NULL, TRUE);
	filter_data->window = window;
	filter_data->prop_name = prop_name;
	filter_data->state = PropertyNewValue;
	gdk_window_add_filter(window, _gdkx_tools_filter, filter_data);
	gdk_window_set_events(window, gdk_window_get_events(window) |GDK_PROPERTY_CHANGE_MASK);
	gdk_error_trap_push();
	XChangeProperty(display, w, property, type, format, mode, data, nelements);
	if(gdk_error_trap_pop()) {
		rt = FALSE;
		goto ex;
	}
	if(g_main_loop_is_running(filter_data->loop)){
		GDK_THREADS_LEAVE();
		g_main_loop_run(filter_data->loop);
		GDK_THREADS_ENTER();
	}
ex:
	gdk_window_remove_filter(window, _gdkx_tools_filter, filter_data);
	g_free(filter_data);
	return rt;
}
gchar * _gdkx_tools_get_window_prop(GdkWindow * window, GdkAtom prop_name, gint * bytes_return){
	Display *display = GDK_DISPLAY_XDISPLAY(gdk_drawable_get_display(window));
	Window w = GDK_WINDOW_XWINDOW(window);
	Atom property = gdk_x11_atom_to_xatom(prop_name);
	long long_offset = 0, long_length = -1;
	Bool delete = FALSE;
	Atom req_type = AnyPropertyType; 
	Atom actual_type_return;
	int actual_format_return;
	unsigned long nitems_return;
	unsigned long bytes_after_return;
	unsigned char *prop_return;
	gchar * rt;
	gdk_error_trap_push();
	XGetWindowProperty(display, w, property, long_offset, long_length, delete, req_type, 
                        &actual_type_return, &actual_format_return, &nitems_return, &bytes_after_return, 
                        &prop_return);
	if(gdk_error_trap_pop()){
		rt = NULL;
		goto ex;
	}
	rt = g_memdup(prop_return, nitems_return);
	if(bytes_return) *bytes_return = nitems_return;
	XFree(prop_return);
ex:
	return rt;
}
