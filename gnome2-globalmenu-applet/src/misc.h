gboolean wnck_window_is_stealable_menubar(WnckWindow * window);
XWindowID menubar_window_get_master(WnckWindow * wnck_menubarwindow);
void gdk_foreign_window_send_resize_event (GdkWindow * window, gint w, gint h);
void
gdk_foreign_window_send_client_message (GdkWindow        *recipient,
              GtkMenuBarMessage			message,
              glong             data0,
              glong             data1,
              glong             data2);
