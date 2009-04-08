using Gtk; 
namespace Gnomenu {
	public class GlobalMenu : MenuBar {
		private Window current_window;
		private Window _root_gnomenu_window;
		construct {
			activate += (menubar, item) => {
				if(current_window != null) {
					current_window.emit_menu_event(item.item_path);
				}
			};
		}
		private HashTable<uint, Gtk.Widget> keys = new HashTable<uint, Gtk.Widget>(direct_hash, direct_equal);

		private void grab_mnemonic_keys() {
			Gdk.ModifierType mods = Gdk.ModifierType.MOD1_MASK;
			foreach(Gtk.Widget widget in get_children()) {
				Gnomenu.MenuItem item = widget as Gnomenu.MenuItem;
				if(item == null) continue;
				Gnomenu.MenuLabel label = item.get_child() as Gnomenu.MenuLabel;
				if(label == null) continue;
				uint keyval = label.mnemonic_keyval;
				message("grabbing key for %s:%u", label.label, keyval);
				if(current_window != null)
					current_window.grab_key(keyval, mods);
				keys.insert(keyval, widget);
			}
		}

		private void ungrab_mnemonic_keys() {
			Gdk.ModifierType mods = Gdk.ModifierType.MOD1_MASK;
			foreach(uint keyval in keys.get_keys()) {
				message("ungrabbing %u", keyval);
				if(current_window != null)
					current_window.ungrab_key(keyval, mods);
			}
			keys.remove_all();
		}

		private void regrab_menu_bar_key() {
			message("regrab menu_bar key");
			ungrab_menu_bar_key();	
			grab_menu_bar_key();	
		}
		private void attach_to_screen(Gdk.Screen screen) {
			_root_gnomenu_window = new Window(get_root_window());
			_root_gnomenu_window.set_key_widget(this.get_toplevel());
			grab_menu_bar_key();
			grab_mnemonic_keys();
			Settings settings = get_settings();
			settings.notify["gtk-menu-bar-accel"] += regrab_menu_bar_key;
				
		}
		private void detach_from_screen(Gdk.Screen screen) {
			if(_root_gnomenu_window != null) {
				_root_gnomenu_window.set_key_widget(null);
				ungrab_menu_bar_key();
				ungrab_mnemonic_keys();
			}
			Settings settings = get_settings();
			settings.notify["gtk-menu-bar-accel"] -= regrab_menu_bar_key;
			_root_gnomenu_window = null;
		}
		public override void hierarchy_changed(Gtk.Widget? old_toplevel) {
			this.get_toplevel().key_press_event += (w, event) => {
				message("key %s", event.str);
				Gtk.Widget widget = keys.lookup(event.keyval);
				if(widget != null) {
					widget.mnemonic_activate(true);
					return true;
				}
				return false;
			};
		}
		public override void screen_changed(Gdk.Screen? previous_screen) {
			Gdk.Screen screen = get_screen();
			if(previous_screen != screen) {
				if(previous_screen != null) detach_from_screen(previous_screen);
				if(screen != null) attach_to_screen(screen);
			}
		}
		public void switch_to(ulong xid) {
			ungrab_mnemonic_keys();
			current_window = Window.foreign_new(xid);
			if(current_window != null) {
				current_window.menu_context_changed += (window) => {
					update();
				};
				update();	
				current_window.set_key_widget(this.get_toplevel());
			}
		}
		private void update() {
			ungrab_mnemonic_keys();
			weak string context = current_window.menu_context;
			if(context != null) {
				try {
					Parser.parse(this, context);
				} catch(GLib.Error e) {
					warning("%s", e.message);	
				}
				show();
				grab_mnemonic_keys();
				return;
			}
			grab_mnemonic_keys();
			hide();
		}
		private void ungrab_menu_bar_key() {
			int keyval = (int) _root_gnomenu_window.get_data("menu-bar-keyval");
			Gdk.ModifierType mods = 
				(Gdk.ModifierType) _root_gnomenu_window.get_data("menu-bar-keymods");

			_root_gnomenu_window.ungrab_key(keyval, mods);
			_root_gnomenu_window.set_data("menu-bar-keyval", null);
			_root_gnomenu_window.set_data("menu-bar-keymods", null);
		}
		private void grab_menu_bar_key() {
			/*FIXME: listen to changes in GTK_SETTINGS.*/
			int keyval;
			Gdk.ModifierType mods;
			get_accel_key(out keyval, out mods);
			_root_gnomenu_window.grab_key(keyval, mods);
			_root_gnomenu_window.set_data("menu-bar-keyval", (void*) keyval);
			_root_gnomenu_window.set_data("menu-bar-keymods", (void*) mods);
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
