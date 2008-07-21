#include <config.h>
#include <glib-2.0/glib.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>

#include "tools.h"

#if ENABLE_TRACING >= 1 || 1
#define LOG(fmt, args...) g_message("<GnomenuTools>::" fmt,  ## args)
#else
#define LOG(fmt, args...)
#endif
#define LOG_FUNC_NAME LOG("%s", __func__)
/**
 * _gdkx_tools_set_window_prop_blocked:
 * 
 * blocked operation, return only if the property is set;
 */
typedef enum {
		GDKX_TOOLS_FILTER_TYPE_PROP,
		GDKX_TOOLS_FILTER_TYPE_SMS,
		GDKX_TOOLS_FILTER_TYPE_KEY,

} GdkXToolsFilterType;
typedef struct _GdkXToolsPropFilterData{
	GdkXToolsFilterType type;
	GdkAtom prop_name;
	GdkWindow * window;
	GMainLoop * loop;
	int state;
	gboolean been_timeout;
} GdkXToolsPropFilterData;

struct _GdkXToolsSMSFilterData {
	GdkXToolsFilterType type;
	GdkXToolsSMSFilterFunc func;
	gpointer data;
	gboolean frozen;
	GdkWindow * window;
	gboolean private_window;
};
struct _GdkXToolsKeyFilterData {
	GdkXToolsFilterType type;
	GdkXToolsKeyFilterFunc func;
	gpointer data;
	gboolean frozen;
};
typedef struct _GdkXToolsSMSFilterData GdkXToolsSMSFilterData;
typedef struct _GdkXToolsKeyFilterData GdkXToolsKeyFilterData;

typedef union _GdkXToolsFilterData {
	GdkXToolsFilterType type;
	GdkXToolsPropFilterData prop;
	GdkXToolsSMSFilterData sms;
	GdkXToolsKeyFilterData key;
} GdkXToolsFilterData;

static GdkFilterReturn _gdkx_tools_client_message_filter(GdkXEvent * xevent, GdkEvent * event, GdkXToolsSMSFilterData * filter_data){
	XClientMessageEvent * x_client_message = xevent;
	if(!filter_data->frozen 
			&& event->any.window == filter_data->window)
		filter_data->func(filter_data->data, x_client_message->data.b, 20);
	return GDK_FILTER_REMOVE;
}

static GdkFilterReturn _gdkx_tools_key_filter(GdkXEvent * xevent, GdkEvent * event, GdkXToolsKeyFilterData * filter_data){
	g_assert_not_reached();
	if(!filter_data->frozen) {
		return filter_data->func(filter_data->data, event);
	}
}

static GdkFilterReturn _gdkx_tools_filter(GdkXEvent * gdkxevent, GdkEvent * event, GdkXToolsFilterData * data){
	XEvent * xevent = gdkxevent;
	if(data->type == GDKX_TOOLS_FILTER_TYPE_PROP && xevent->type !=PropertyNotify){
		g_message("message removed");
		return GDK_FILTER_REMOVE;
	}
	switch(xevent->type){
		case PropertyNotify:
		if(data->type == GDKX_TOOLS_FILTER_TYPE_PROP 
		&& xevent->xproperty.window == GDK_WINDOW_XWINDOW(data->prop.window) 
		&& xevent->xproperty.state == data->prop.state 
		&& xevent->xproperty.atom == gdk_x11_atom_to_xatom(data->prop.prop_name))
			g_main_loop_quit(data->prop.loop);
		return GDK_FILTER_CONTINUE;
		case ClientMessage:
		if(data->type == GDKX_TOOLS_FILTER_TYPE_SMS
		&& xevent->xclient.message_type 
		== gdk_x11_atom_to_xatom(gdk_atom_intern("GNOMENU_SMS", FALSE)))
		return _gdkx_tools_client_message_filter(gdkxevent, event, data);
		case KeyPress:
		if(data->type == GDKX_TOOLS_FILTER_TYPE_KEY)
		return _gdkx_tools_key_filter(gdkxevent, event, data);
	}
	return GDK_FILTER_CONTINUE;
}
static gboolean _gdkx_tools_timeout(GdkXToolsPropFilterData * filter_data){
	filter_data->been_timeout = TRUE;
	g_main_loop_quit(filter_data->loop);
	return FALSE;
}
gboolean gdkx_tools_set_window_prop(GdkWindow * window, const gchar * prop_name, gchar * buffer, gint size){
	gboolean rt = TRUE;
	Display *display = GDK_DISPLAY_XDISPLAY(gdk_drawable_get_display(window));
	Window w = GDK_WINDOW_XWINDOW(window);
	Atom property = gdk_x11_atom_to_xatom(gdk_atom_intern(prop_name, FALSE));
   	Atom type = gdk_x11_atom_to_xatom(gdk_atom_intern(prop_name, FALSE));
	int format = 8;
	int mode = PropModeReplace;
	unsigned char *data = buffer;
	int nelements = size;
	gdk_error_trap_push();
	XChangeProperty(display, w, property, type, format, mode, data, nelements);
	if(gdk_error_trap_pop()) {
		rt = FALSE;
	}
	return rt;
}
gboolean gdkx_tools_set_window_prop_blocked(GdkWindow * window, const gchar * prop_name, gchar * buffer, gint size){
	GdkXToolsPropFilterData * filter_data = g_new0(GdkXToolsFilterData,1);
	gboolean rt = TRUE;
	Display *display = GDK_DISPLAY_XDISPLAY(gdk_drawable_get_display(window));
	Window w = GDK_WINDOW_XWINDOW(window);
	Atom property = gdk_x11_atom_to_xatom(gdk_atom_intern(prop_name, FALSE));
   	Atom type = gdk_x11_atom_to_xatom(gdk_atom_intern(prop_name, FALSE));
	int format = 8;
	int mode = PropModeReplace;
	unsigned char *data = buffer;
	int nelements = size;
	filter_data->type = GDKX_TOOLS_FILTER_TYPE_PROP;
	filter_data->loop = g_main_loop_new(NULL, TRUE);
	filter_data->window = window;
	filter_data->prop_name = gdk_atom_intern(prop_name, FALSE);
	filter_data->state = PropertyNewValue;
	filter_data->been_timeout = FALSE;
	guint timeout_id = g_timeout_add_seconds(3, _gdkx_tools_timeout, filter_data);
	gdk_window_add_filter(window, _gdkx_tools_filter, filter_data);
	gdk_window_set_events(window, gdk_window_get_events(window) |GDK_PROPERTY_CHANGE_MASK);
	gdk_error_trap_push();
	XChangeProperty(display, w, property, type, format, mode, data, nelements);
	
	XSync(display, TRUE);
	if(gdk_error_trap_pop()) {
		rt = FALSE;
		goto ex;
	}
	/*
	if(g_main_loop_is_running(filter_data->loop)){
		GDK_THREADS_LEAVE();
		g_main_loop_run(filter_data->loop);
		GDK_THREADS_ENTER();
	}
	*/
	
ex:
	g_source_remove(timeout_id);
	gdk_window_remove_filter(window, _gdkx_tools_filter, filter_data);
	rt = filter_data->been_timeout;
	g_free(filter_data);
	return rt;
}
gchar * gdkx_tools_get_window_prop(GdkWindow * window, const gchar * prop_name, gint * bytes_return){
	Display *display = GDK_DISPLAY_XDISPLAY(gdk_drawable_get_display(window));
	Window w = GDK_WINDOW_XWINDOW(window);
	Atom property = gdk_x11_atom_to_xatom(gdk_atom_intern(prop_name, FALSE));
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

/**
 * _gnomenu_socket_find_targets:
 * @self: self
 * @name: the name for the targets
 * 
 * Find every possible @GnomenuSocket on this display with the name @name.
 *
 * Returns: a GList contains the list of those sockets' native ID. 
 * It is the caller's obligation to free the list.
 */
static GList * _gnomenu_socket_find_targets(GdkScreen * screen, gchar * name){
	GList * window_list = NULL;
	GdkWindow * root_window;
	GdkDisplay * display;
    Window root_return;
    Window parent_return;
    Window * children_return;
    unsigned int nchildren_return;
    unsigned int i;

	g_return_val_if_fail(screen != NULL, NULL);

	root_window = gdk_screen_get_root_window(screen);
	display = gdk_screen_get_display(screen);
	g_return_val_if_fail(root_window != NULL, NULL);

    gdk_error_trap_push();
    XQueryTree(GDK_DISPLAY_XDISPLAY(display),
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
        Atom type_req = gdk_x11_get_xatom_by_name_for_display (display, "UTF8_STRING");
        gint format_return;
        gulong nitems_return;
        gulong bytes_after_return;
        guchar * wm_name;
        gint rt;
        gdk_error_trap_push();
        rt = XGetWindowProperty (GDK_DISPLAY_XDISPLAY (display), children_return[i],
                          gdk_x11_get_xatom_by_name_for_display (display, "_NET_WM_NAME"),
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
	return window_list;
}
gboolean gdkx_tools_send_sms(gchar * sms, int size){
	GList * list = _gnomenu_socket_find_targets(gdk_screen_get_default(), "GNOMENU_SMS_LISTENER");
	GList * node;
	for(node = list; node; node = node->next)
		gdkx_tools_send_sms_to(node->data, sms, size);
	g_list_free(list);
	return TRUE;
}
gboolean gdkx_tools_send_sms_to(GdkNativeWindow target, gchar * sms, int size){
	GdkEventClient event;
	int i;
	event.type = GDK_CLIENT_EVENT;
	event.window = gdk_get_default_root_window();
	event.send_event = TRUE;
	event.message_type = gdk_atom_intern("GNOMENU_SMS", FALSE);
	event.data_format = 8;

	g_message("%d", size);
	for(i=0; i< size && i<20; i++){
		event.data.b[i] = sms[i];
	}
	GString * string = g_string_new("");
	g_string_append(string, "sending sms:");
	g_string_append_printf(string, "%d, ", event.data.b[0]);
	for(i = 1; i< 18; i++){
		guint b = (guchar)event.data.b[i];
		g_string_append_printf(string, "%0.2X ", b);
	}
	LOG("%s", string->str);
	g_string_free(string, TRUE);
	gdk_event_send_client_message(&event, target);
	gdk_display_sync(gdk_display_get_default());
	return TRUE;

}
static GList * sms_filter_list = NULL;
static GdkXToolsSMSFilterData * _gdkx_tools_find_sms_filter(GdkXToolsSMSFilterFunc func, gpointer data){
	GdkXToolsSMSFilterData * filter_data;
	GList * node;
	for(node = sms_filter_list; node ; node = node->next){
		filter_data = node->data;
		if(filter_data->data == data &&
			filter_data->func == func){
			return filter_data;
		} 
	}
	return NULL;
}
GdkWindow * gdkx_tools_add_sms_filter(GdkWindow * window, GdkXToolsSMSFilterFunc func, gpointer data, gboolean frozen){
	GdkXToolsSMSFilterData * filter_data;
	GdkWindowAttr attr = {0};
	/*FIXME: THREAD SAFETY! protect the list!*/
	GDK_THREADS_ENTER();
	filter_data =  _gdkx_tools_find_sms_filter(func, data);

	if(filter_data != NULL){
		GDK_THREADS_LEAVE();
		g_warning("filter already registered");
		return;
	}

   	filter_data = g_new0(GdkXToolsFilterData, 1);
	filter_data->type = GDKX_TOOLS_FILTER_TYPE_SMS;
	filter_data->func = func;
	filter_data->data = data;
	filter_data->frozen = frozen;
	filter_data->private_window = FALSE;
	if(!window ) {
		attr.wclass = GDK_INPUT_ONLY;
		attr.title = "GNOMENU_SMS_LISTENER";
		filter_data->window = gdk_window_new(gdk_get_default_root_window(), &attr, GDK_WA_TITLE);
		filter_data->private_window = TRUE;
	} else
		filter_data->window = window;
	/*FIXME: THREAD SAFETY!*/
	sms_filter_list = g_list_append(sms_filter_list, filter_data);
	gdk_window_add_filter(filter_data->window, _gdkx_tools_filter, filter_data);
	GDK_THREADS_LEAVE();
	return filter_data->window;
}

void gdkx_tools_freeze_sms_filter(GdkXToolsSMSFilterFunc func, gpointer data){
	GdkXToolsSMSFilterData * filter_data;
	GDK_THREADS_ENTER();
	filter_data = _gdkx_tools_find_sms_filter(func, data);
	if(filter_data){
		filter_data->frozen = TRUE;
	} else {
		g_warning("filter is not registered");
	}
	GDK_THREADS_LEAVE();
}
void gdkx_tools_thaw_sms_filter(GdkXToolsSMSFilterFunc func, gpointer data){
	GdkXToolsSMSFilterData * filter_data;
	GDK_THREADS_ENTER();
	filter_data = _gdkx_tools_find_sms_filter(func, data);

	if(filter_data){
		filter_data->frozen = FALSE;
	} else {
		g_warning("filter is not registered");
	}
	GDK_THREADS_LEAVE();
}
void gdkx_tools_remove_sms_filter(GdkXToolsSMSFilterFunc func, gpointer data){
	GdkXToolsSMSFilterData * filter_data;
	GDK_THREADS_ENTER();
	filter_data = _gdkx_tools_find_sms_filter(func, data);

	if(filter_data){
		gdk_window_remove_filter(filter_data->window, _gdkx_tools_filter, filter_data);
		sms_filter_list = g_list_remove(sms_filter_list, filter_data);
		if(filter_data->private_window) {
			gdk_window_destroy(filter_data->window);
		}
		g_free(filter_data);
	} else {
		g_warning("filter is not registered");
	}
	GDK_THREADS_LEAVE();
}
void gdkx_tools_add_key_filter(GdkXToolsKeyFilterFunc func, gpointer data) {

}

void gdkx_tools_remove_key_filter(GdkXToolsKeyFilterFunc func, gpointer data) {


}
void
gdkx_tools_grab_key (int      keycode,
                guint    modifiers)
{
	GdkWindow *root;

	root = gdk_get_default_root_window ();

	XGrabKey (GDK_DISPLAY_XDISPLAY (gdk_display_get_default()),
		  keycode, modifiers,
		  GDK_WINDOW_XWINDOW (root),
		  True, GrabModeAsync, GrabModeAsync);
}
void
gdkx_tools_ungrab_key (int      keycode,
                guint    modifiers) {
	GdkWindow *root;

	root = gdk_get_default_root_window ();

	XUngrabKey (GDK_DISPLAY_XDISPLAY (gdk_display_get_default()),
			keycode, modifiers,
		  GDK_WINDOW_XWINDOW (root));

}
GdkWindow * gdkx_tools_lookup_window(GdkNativeWindow key){
	GdkWindow * rt = gdk_window_lookup(key);
	if(!rt) rt = gdk_window_foreign_new(key);
	return rt;
}
gboolean gdkx_tools_remove_window_prop(GdkWindow * window, const gchar * prop_name) {
	gdk_property_delete(window, gdk_atom_intern(prop_name, FALSE));
	return TRUE;
}
