namespace Gdk {
[CCode (cname = "GDK_WINDOW_XID", cheader_filename="gdk/gdkx.h")]
public static ulong XWINDOW(Gdk.Drawable drawable);
}

[CCode (cprefix = "Gtk", lower_case_cprefix = "gtk_")]
namespace GtkCompat {
	[CCode (cheader_filename = "gtk/gtk.h", cname="GtkNotebook")]
	public class Notebook : Gtk.Container, Atk.Implementor, Gtk.Buildable {
		public int page_num(Gtk.Widget child);
	}
	[CCode (cheader_filename = "gtk/gtk.h")]
	public class Container : Gtk.Widget, Atk.Implementor, Gtk.Buildable {
		public virtual void forall (Gtk.Callback callback);
	}
	[CCode (cheader_filename = "gtk/gtk.h")]
	public class Widget : Gtk.Object, Atk.Implementor, Gtk.Buildable {
		public virtual signal void style_set (Gtk.Style? previous_style);
	}


}
