[CCode (cheader_filename = "dyn-patch.h")]
namespace DynPatch {
[CCode (cname = "dyn_patch_init")]
	public void init();
[CCode (cname = "dyn_patch_uninit")]
	public void uninit();
[CCode (cname = "dyn_patch_get_window")]
	public weak Gtk.Window? get_window(Gtk.MenuBar menubar);
[CCode (cname = "dyn_patch_get_menubar")]
	public weak Gtk.MenuBar? get_menubar(Gtk.Widget widget);
}
