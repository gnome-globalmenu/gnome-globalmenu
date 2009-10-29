public class Gnomenu.GlobalMenuItem : Gtk.MenuItem {
	private Gnomenu.Monitor active_window_monitor;

	public bool per_monitor_mode {
		get { return active_window_monitor.per_monitor_mode;}
		set {
			active_window_monitor.per_monitor_mode = value;
		}
	}
	public Gnomenu.Window active_window { 
		get {
			return active_window_monitor.active_window;
		}
	}

	public signal void active_window_changed(Gnomenu.Window? prev_window);

	private Gnomenu.Menu main_shell = null;
	construct {
		this.label = _("Menu");
		active_window_monitor = new Gnomenu.Monitor(this.get_screen());
		active_window_monitor.managed_shell = null;
		active_window_monitor.monitor_num = -1;
		active_window_monitor.active_window_changed += emit_active_window_changed;
		main_shell = new Gnomenu.Menu();
		this.submenu = main_shell;
		main_shell.is_topmost = true;
		main_shell.activate += item_activated;
		main_shell.select += item_selected;
		main_shell.deselect += item_deselected;
		this.hierarchy_changed += _hierarchy_changed;
		this.activate += () => {
			active_window_monitor.rebuild_shell(main_shell);
		};
	}

	private void emit_active_window_changed(Gnomenu.Window? prev_window) {
		active_window_changed(prev_window);
	}

	private void item_activated (Gnomenu.Item item){
		if(active_window != null) {
			active_window.emit_menu_event(item.item_path);
		}
	}
	private void item_selected (Gnomenu.Item item){
		if(active_window != null) {
			active_window.emit_menu_select(item.item_path, null);
		}
	}
	private void item_deselected (Gnomenu.Item item){
		if(active_window != null) {
			active_window.emit_menu_deselect(item.item_path);
		}
	}

	private void _hierarchy_changed(Gtk.Widget? old_toplevel) {
		var toplevel = this.get_toplevel() as Gtk.Plug;
		if(toplevel != null) {
			toplevel.configure_event += sync_monitor_num;
		}
		if(old_toplevel != null)
			old_toplevel.configure_event -= sync_monitor_num;
	}

	private bool sync_monitor_num() {
		var screen = get_screen();
		if(this.is_realized()) {
		active_window_monitor.monitor_num = 
		screen.get_monitor_at_window(this.window);
		} else {
			active_window_monitor.monitor_num = -1;
		}
		return false;
	}

}
