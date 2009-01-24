using Gtk; 
namespace Gnomenu {
	public class GlobalMenu : MenuBar {
		private Window current_window;
		construct {
			activate += (menubar, item) => {
				if(current_window != null) {
					current_window.emit_menu_event(item.path);
				}
			};
		}
		public void switch_to(ulong xid) {
			if(current_window != null) {
				current_window.destroy();
			}
			current_window = Window.new_from_native(xid);
			if(current_window != null) {
				current_window.menu_context_changed += (window) => {
					update();
				};
				update();	
			}
		}
		private void update() {
			string context = current_window.menu_context;
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
	}
}
