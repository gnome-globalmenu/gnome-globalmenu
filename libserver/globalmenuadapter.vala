
public class Gnomenu.GlobalMenuAdapter : Gnomenu.Adapter {

	public GlobalMenuAdapter(Gtk.MenuShell gtk_shell) {
		this.gtk_shell = gtk_shell;
	}

	public Gnomenu.Window active_window { 
		get {
			return active_window_monitor.active_window;
		}
	}

	public signal void active_window_changed(Gnomenu.Window? prev_window);

	private Gnomenu.Monitor active_window_monitor;
	private Gnomenu.MnemonicKeys mnemonic_keys;
	construct {
		mnemonic_keys = new MnemonicKeys(this);
		active_window_monitor = new Gnomenu.Monitor(gtk_shell.get_screen());
		active_window_monitor.managed_shell = this;
		active_window_monitor.monitor_num = -1;
		active_window_monitor.active_window_changed += emit_active_window_changed;
		active_window_monitor.active_window_changed += regrab_mnemonic_keys;
		active_window_monitor.shell_rebuilt += regrab_mnemonic_keys;
		active_window_monitor.active_window_lost_focus += regrab_mnemonic_keys;
		active_window_monitor.active_window_received_focus += regrab_mnemonic_keys;
		this.activate += item_activated;
		this.select += item_selected;
		this.deselect += item_deselected;
		gtk_shell.hierarchy_changed += _hierarchy_changed;
		gtk_shell.hierarchy_changed += _hierarchy_changed_chain_keys;
	}

	private void regrab_mnemonic_keys() {
		mnemonic_keys.grab(active_window);
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
		var toplevel = gtk_shell.get_toplevel() as Gtk.Plug;
		if(toplevel != null) {
			toplevel.configure_event += sync_monitor_num;
		}
		if(old_toplevel != null)
			old_toplevel.configure_event -= sync_monitor_num;
	}

	private bool sync_monitor_num() {
		var screen = gtk_shell.get_screen();
		if(gtk_shell.is_realized()) {
		active_window_monitor.monitor_num = 
		screen.get_monitor_at_window(gtk_shell.window);
		} else {
			active_window_monitor.monitor_num = -1;
		}
		return false;
	}

	private void chainup_key_changed(Gtk.Window window) {
		GLib.Type type = typeof(Gtk.Window);
		var window_class = (Gtk.WindowClass) type.class_ref();
		debug("chainup to Gtk.Window keys changed");
		window_class.keys_changed(window);
	}

	private void _hierarchy_changed_chain_keys(Gtk.Widget? old_toplevel) {
		var toplevel = gtk_shell.get_toplevel() as Gtk.Plug;
		if(toplevel != null) {
		/* Manually chain-up to the default keys_changed handler,
		 * Working around a problem with GtkPlug/GtkSocket */
			toplevel.keys_changed += chainup_key_changed;
		}
		if((old_toplevel as Gtk.Plug)!= null) {
		/* Manually chain-up to the default keys_changed handler,
		 * Working around a problem with GtkPlug/GtkSocket */
			(old_toplevel as Gtk.Plug).keys_changed -= chainup_key_changed;
		}
	}

}
