using GLib;
using Gtk;
using GtkCompat;

[CCode (cprefix = "GMarkup", lower_case_cprefix = "g_markup_")]
namespace GMarkup {
	public View: GLib.Object {
		public abstract weak Node? root {get; set;}
	}
}
