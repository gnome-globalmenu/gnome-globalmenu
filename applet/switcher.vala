using GLib;
using Gnomenu;
using Gtk;
using GnomeMenuHelper;
public extern string* __get_task_name_by_pid(int pid);
namespace GnomenuExtra {
	
	public class Switcher : Gnomenu.MenuBar {
		private string _label;
		private int _max_size = 30;
		private bool _show_icon = false;
		private bool _show_label = true;
		private Gdk.Pixbuf _icon;
		private Wnck.Window _window;
		
		private const string TEMPLATE = """<menu><item label="%s" font="bold"/></menu>""";
		private GLib.HashTable<string,string> program_list;
		
		public Switcher() {
			Parser.parse(this, TEMPLATE.replace("%s","Global Menu Bar"));
			program_list = GnomeMenuHelper.get_flat_list();
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
			if (process_name==null) return window.get_name();
			
			string ret;
			if (program_list.lookup(process_name)!=null)
				ret = program_list.lookup(process_name); else
				ret = window.get_name();
			return ret; 
		}
		private string cut_string(string txt, int max) {
			if (max<1) return txt;
			if (max<=3) return "...";
			if (txt.length>max) return txt.substring(0, (max-3)) + "...";
			return txt;
		}
		public void update(Wnck.Window? window=_window) {
			_window = window;
			_label = get_application_name(window);
			if (_show_label) 
				Parser.parse(this, TEMPLATE.replace("%s", cut_string(_label, _max_size))); else
				Parser.parse(this, TEMPLATE.replace("%s", ""));
				
			_icon = window.get_mini_icon();
			if (_show_icon)
				this.get("/0").icon_pixbuf  = _icon; else
				this.get("/0").item_type = "normal";
				
			this.visible = (_show_icon | _show_label);
		}
		public int max_size {
			get { return _max_size; }
			set {
				_max_size = value;
				update();
			}
		}
		public bool show_icon {
			get { return _show_icon; }
			set {
				_show_icon = value;
				update();
			}
		}
		public bool show_label {
			get { return _show_label; }
			set {
				_show_label = value;
				update();
			}
		}
	}
}
