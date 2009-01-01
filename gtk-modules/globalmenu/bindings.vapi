public static GLib.List<weak Gtk.Widget> gtk_container_get_children(Gtk.Container container);
public static GLib.List<weak Gtk.Window> gtk_window_list_toplevels();
[CCode (cname="g_dgettext", cheader_filename="glib.h,glib/gi18n-lib.h")]
public weak string dgettext(string domain, string msgid);

public GLib.LogFunc g_log_default_handler;
[CCode (cname="GDK_DRAWABLE_XID", cheader_filename="gdk/gdkx.h")]
public ulong gdk_drawable_xid(Gdk.Drawable drawable);
