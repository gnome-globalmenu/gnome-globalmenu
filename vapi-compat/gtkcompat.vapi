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
		[CCode (cname = "gtk_container_forall")]
		public void forall_children (Gtk.Callback callback);
		public virtual void forall (bool include_internals, Callback callback, void* data);
	}
	[CCode (cheader_filename = "gtk/gtk.h")]
	public class Widget : Gtk.Object, Atk.Implementor, Gtk.Buildable {
		public virtual signal void style_set (Gtk.Style? previous_style);
		public virtual signal void hierarchy_changed (Gtk.Widget? old_toplevel);
	}

	public static delegate void Callback(Gtk.Widget widget, void * data);

}
