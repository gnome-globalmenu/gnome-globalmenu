using GLib;
using Gnomenu;
using Gtk;
public extern string* __get_task_name_by_pid(int pid);
namespace GnomenuExtra {
	
	public class Switcher : Gnomenu.MenuBar {
		private string _label;
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
		private string remove_path(string txt, string separator) {
			long co = txt.length-1;
			while ((co>=0) && (txt.substring(co, 1)!=separator)) {
				co--;
			}
			string ret = txt.substring(co+1,(txt.length-co-1));
			return ret;
		}
		private string get_process_name(Wnck.Window window) {
			string txt = __get_task_name_by_pid(window.get_application().get_pid());
			if ((txt==null) || (txt=="")) return window.get_application().get_name();
			string ret = txt.chomp();
			if (ret.substring(ret.length-4,4)==".exe") return remove_path(ret, "\\"); // is a wine program

			ret = remove_path(ret.split(" ")[0], "/");
				
			switch(ret) {
			case "mono":
			case "python":
			case "python2.5":
			case "vmplayer":
				return remove_path(txt.chomp().split(" ")[1], "/");
				break;
			case "wine":
				return window.get_application().get_name();
				break;
			}
			return ret;
		}
		private string get_application_name(Wnck.Window window) {
			if (window.get_window_type() == Wnck.WindowType.DESKTOP)
				return "Desktop";
				
			string process_name = get_process_name(window);
			return process_name;
		}
		public void update(Wnck.Window? window) {
			_label = get_application_name(window);
			Parser.parse(this, TEMPLATE.printf(_label));
			forceBold(this.get("/0")); /* TOFIX */
		}
	}
}
