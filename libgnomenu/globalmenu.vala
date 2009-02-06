using Gtk; 
namespace Gnomenu {
	public class GlobalMenu : MenuBar {
		private Window current_window;
		private Window _root_gnomenu_window;
		construct {
			activate += (menubar, item) => {
				if(current_window != null) {
					current_window.emit_menu_event(item.path);
				}
			};
			key_press_event += (t, event) => {
				uint keyval;
				Gdk.ModifierType mods;
				get_accel_key(out keyval, out mods);
				if(event.keyval == keyval &&
					(event.state & Gtk.accelerator_get_default_mod_mask())
					== (mods & Gtk.accelerator_get_default_mod_mask())) {
					/* We chain up to the toplevel key_press_event,
					 * which is listened by all the menubars within
					 * the applet*/
					Gtk.Widget toplevel = get_toplevel();
					if(toplevel != null) 
						toplevel.key_press_event(event);
					return false;
				}
				return true;
			};
		}
		private void attach_to_screen(Gdk.Screen screen) {
			_root_gnomenu_window = new Window(get_root_window());
			_root_gnomenu_window.set_key_widget(this);
			grab_menu_bar_key(_root_gnomenu_window);
		}
		private void detach_from_screen(Gdk.Screen screen) {
			if(_root_gnomenu_window != null) {
				ungrab_menu_bar_key(_root_gnomenu_window);
			}
			_root_gnomenu_window = null;
		}
		public override void screen_changed(Gdk.Screen previous_screen) {
			Gdk.Screen screen = get_screen();
			if(previous_screen != screen) {
				if(previous_screen != null) detach_from_screen(previous_screen);
				if(screen != null) attach_to_screen(screen);
			}
		}
		public void switch_to(ulong xid) {
			current_window = Window.foreign_new(xid);
			if(current_window != null) {
				current_window.menu_context_changed += (window) => {
					update();
				};
				update();	
			}
		}
		private void update() {
			weak string context = current_window.menu_context;
			if(context != null) {
				try {
					Parser.parse(this, context);
				} catch(GLib.Error e) {
					warning("%s", e.message);	
				}
				show();
				return;
			}
			hide();
		}
		private void ungrab_menu_bar_key(Gnomenu.Window window) {
			int keyval = (int) window.get_data("menu-bar-keyval");
			Gdk.ModifierType mods = 
				(Gdk.ModifierType) window.get_data("menu-bar-keymods");

			window.ungrab_key(keyval, mods);
			window.set_data("menu-bar-keyval", null);
			window.set_data("menu-bar-keymods", null);
		}
		private void grab_menu_bar_key(Gnomenu.Window window) {
			/*FIXME: listen to changes in GTK_SETTINGS.*/
			int keyval;
			Gdk.ModifierType mods;
			get_accel_key(out keyval, out mods);
			window.grab_key(keyval, mods);
			window.set_data("menu-bar-keyval", (void*) keyval);
			window.set_data("menu-bar-keymods", (void*) mods);
		}	
		/**
		 * return the accelerator key combination for invoking menu bars
		 * in GTK Settings. It is usually F10.
		 */
		private void get_accel_key(out uint keyval, out Gdk.ModifierType mods) {
			Gtk.Settings settings = get_settings();
			string accel = null;
			settings.get("gtk_menu_bar_accel", &accel, null);
			if(accel != null)
				Gtk.accelerator_parse(accel, out keyval, out mods);
		}
	}
}
