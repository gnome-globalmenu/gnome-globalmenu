public class Gnomenu.LocalSettings : Gnomenu.Settings {
	public Gnomenu.GlobalSettings global {get; private set;}

	public override bool show_local_menu {
		get {
			if(global == null) return base.show_local_menu;
			return global.show_local_menu || base.show_local_menu;
		}
		set {
			base.show_local_menu = value;
		}
	}
	public override bool show_menu_icons {
		get {
			if(global == null) return base.show_menu_icons;
			return global.show_menu_icons && base.show_menu_icons;
		}
		set {
			base.show_menu_icons = value;
		}
	}
	public override int changed_notify_timeout {
		get {
			if(global == null) return base.changed_notify_timeout;
			if(global.changed_notify_timeout < base.changed_notify_timeout) {
				return global.changed_notify_timeout;
			}
			return base.changed_notify_timeout;
		}
		set {
			base.changed_notify_timeout = value;
		}
	}

	public LocalSettings(Gdk.Window? window = null) {
		attach_to_window(window);
		this.show_local_menu = false;
	}
	public override void attach_to_window(Gdk.Window? window = null) {
		base.attach_to_window(window);
		if(window != null) 
			global = Gnomenu.GlobalSettings.get(window.get_screen());
		
	}
}
