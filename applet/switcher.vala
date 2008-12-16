using GLib;
using Gnomenu;
using Gtk;
namespace GnomenuExtra {
	public class Switcher : Gnomenu.MenuBar {
		private const string TEMPLATE = """<menu><item label="%s" font="bold"/></menu>""";
		public Switcher() {
			Parser.parse(this, TEMPLATE.printf("Global Menu Bar"));
			forceBold(this.get("/0"));	/* TOFIX */
		}
		
		/* This is a TEMPORARY workaround while waiting the issue in Gnomenu.MenuItem being fixed */
		private void forceBold(Gnomenu.MenuItem mi) {
			MenuLabel? menulabel = mi.get_child() as MenuLabel;
			menulabel.label_widget.set_markup_with_mnemonic("<b>" + menulabel.label_widget.label + "</b>");
		}
		
		public void update(Wnck.Window? window) {
			Parser.parse(this, TEMPLATE.printf(window.get_application().get_name()));
			forceBold(this.get("/0")); /* TOFIX */
		}
	}
}
