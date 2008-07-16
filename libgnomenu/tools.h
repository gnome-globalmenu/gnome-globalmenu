gboolean gdkx_tools_set_window_prop_blocked(GdkWindow * window, GdkAtom prop_name, gchar * buffer, gint size);
gchar * gdkx_tools_get_window_prop(GdkWindow * window, GdkAtom prop_name, gint * bytes_return);
typedef void (* GdkXToolsSMSFilterFunc)(gpointer data, gchar * sms, int size);
typedef GdkFilterReturn (* GdkXToolsKeyFilterFunc)(gpointer data, GdkEventKey * event);

gboolean gdkx_tools_send_sms(gchar * sms, int size);
gboolean gdkx_tools_send_sms_to(GdkNativeWindow target, gchar * sms, int size);
void gdkx_tools_add_sms_filter(GdkWindow * window, GdkXToolsSMSFilterFunc func, gpointer data, gboolean frozen);
void gdkx_tools_thaw_sms_filter(GdkXToolsSMSFilterFunc func, gpointer data);
void gdkx_tools_freeze_sms_filter(GdkXToolsSMSFilterFunc func, gpointer data);
void gdkx_tools_remove_sms_filter(GdkXToolsSMSFilterFunc func, gpointer data);
