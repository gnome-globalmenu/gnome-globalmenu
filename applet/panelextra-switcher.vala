using GLib;
using Gtk;
using GMarkup;

namespace PanelExtra {
	public class Switcher : PanelExtra.MenuBar {
		private Gtk.ImageMenuItem mi_application;
		private Wnck.Screen screen;
		public int max_size = 45;
		public bool show_label = true;
		public bool show_icon = true;
		public bool show_window_list = true;
		public bool show_window_actions = true;
		
		private string label_text;
		private Gtk.Widget label_icon;
		
		private void app_selected(Gtk.ImageMenuItem? item) {
			if (((item.user_data as Wnck.Window).is_active()) && ((item.user_data as Wnck.Window).is_visible_on_workspace((item.user_data as Wnck.Window).get_workspace()))) {
				(item.user_data as Wnck.Window).minimize();
				return;
			}

			// Ensure viewport visibility
			int current_workspace_x = (item.user_data as Wnck.Window).get_workspace().get_viewport_x();
			int current_workspace_y = (item.user_data as Wnck.Window).get_workspace().get_viewport_y();
			int x,y,w,h;
			(item.user_data as WnckCompat.Window).get_geometry(out x, out y, out w, out h);
			(item.user_data as Wnck.Window).get_screen().move_viewport(current_workspace_x + x, current_workspace_y + y);
			
			(item.user_data as Wnck.Window).activate(Gtk.get_current_event_time());
			(item.user_data as Wnck.Window).get_workspace().activate(Gtk.get_current_event_time());
			(item.user_data as Wnck.Window).unminimize(Gtk.get_current_event_time());
			
			// ensure is on top
			(item.user_data as Wnck.Window).make_above();
			(item.user_data as Wnck.Window).unmake_above();

			//TOFIX: if the window is on another workspace and it is minimized, it doesn't unminimize automatically.
		}
		
		public static void message(string msg) {
			Gtk.MessageDialog m = new Gtk.MessageDialog(null,
							    Gtk.DialogFlags.MODAL,
							    Gtk.MessageType.INFO,
							    Gtk.ButtonsType.OK,
							    msg);
			m.run();
			m.destroy();
		}
		private void refresh_applications_list(Gtk.MenuItem? mi_parent) {
			Wnck.Window window = Wnck.Screen.get_default().get_active_window();
			Gtk.Menu menu;
			
			if ((window.get_window_type()!=Wnck.WindowType.DESKTOP) && (show_window_actions))
				menu = new Wnck.ActionMenu(window); else
				menu = new Gtk.Menu();
			
			if (show_window_list) {
				menu.insert(new Gtk.SeparatorMenuItem(), 0);
				weak GLib.List<Wnck.Window> windows = screen.get_windows();
				foreach(weak Wnck.Window window in windows) {
					if (!window.is_skip_pager()) {
						Gtk.ImageMenuItem mi;
						string txt = window.get_name();
						if ((txt.length>max_size) && (max_size>3)) txt = txt.substring(0, (max_size-3)) + "...";
						mi = new Gtk.ImageMenuItem.with_label(txt);
						if (window.is_active()) set_menu_item_label_bold(mi, "");			
						mi.set_image(new Gtk.Image.from_pixbuf(window.get_mini_icon()));
						mi.user_data = window;
						mi.activate += app_selected;
						menu.insert(mi, 0);
					}
				}
			}
			
			menu.show_all();
			mi_parent.submenu = menu;
		}
		private void set_menu_item_label_bold(Gtk.MenuItem? mi, string text) {
			if (text == "")
				(mi.child as Gtk.Label).set_markup_with_mnemonic("<b>" + (mi.child as Gtk.Label).text + "</b>"); else
				(mi.child as Gtk.Label).set_markup_with_mnemonic("<b>" + text + "</b>");
		}
		private void set_menu_item_label_visible(Gtk.MenuItem? mi, bool value) {
			(mi.child as Gtk.Label).visible = value;
		}
		public void set_label(string text) {
			label_text = text;
			refresh();
		}
		
		private void set_menu_item_icon_visible(Gtk.ImageMenuItem? mi, bool value) {
			if (show_icon) mi.set_image(label_icon); else mi.set_image(null);
		}
		public void set_icon(Gtk.Widget icon) {
			label_icon = icon;
			refresh();
		}
		public void refresh() {
			// set length of label
			string txt = label_text;
			if ((txt.length>max_size) && (max_size>3)) txt = txt.substring(0, (max_size-3)) + "...";
			set_menu_item_label_bold(mi_application, txt);
			
			// set label visibility
			set_menu_item_label_visible(mi_application, show_label);
			
			// set icon visibility
			set_menu_item_icon_visible(mi_application, show_icon);
			
			// refresh menu
			refresh_applications_list(mi_application);
			
			this.visible = (show_icon | show_label);
		}
		public Switcher() { }
		construct {
			mi_application = new Gtk.ImageMenuItem.with_label("GlobalMenu");
			set_menu_item_label_bold(mi_application, "");
			this.add(mi_application);
			this.show_all();
			
			screen = Wnck.Screen.get_default();
			(screen as WnckCompat.Screen).active_window_changed += (screen, previous_window) => {
				refresh_applications_list(mi_application);
			};
			(screen as Wnck.Screen).window_closed += (window) => {
				refresh_applications_list(mi_application);
			};
			(screen as Wnck.Screen).window_opened += (window) => {
				refresh_applications_list(mi_application);
			};
		}
	}
}
