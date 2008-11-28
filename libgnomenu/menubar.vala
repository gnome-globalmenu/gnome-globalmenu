using Gtk;

namespace Gnomenu {
	public class MenuBar : MenuShell {
		public MenuBar() { 
		}	
		public override bool selecting {
			get {
				return base.selecting;
			}	
			set {
				if(value == true) {
					Gdk.pointer_grab(
							event_window,
							true,
							Gdk.EventMask.BUTTON_PRESS_MASK,
							null,
							null,
							Gdk.CURRENT_TIME);
				} else {
					Gdk.pointer_ungrab(Gdk.CURRENT_TIME);
				}
				base.selecting = value;
			}
		}
	}
}
