using GLib;
using Gtk;
using Wnck;

class WorkspaceSelector : Gtk.Window {
	private Wnck.Window? target;
	public WorkspaceSelector(Wnck.Window? window) {
		target = window;
		this.name = "WorkspaceSelector";
        this.set_type_hint(Gdk.WindowTypeHint.DOCK);
        this.window_position = Gtk.WindowPosition.CENTER_ALWAYS;
        this.resizable = false;
        this.allow_grow = false;
        this.decorated = false;
        this.skip_pager_hint = true;
        this.skip_taskbar_hint = true;
        if ((this.child != null)) {
            this.child.show_all();
        }
        this.default_width = 100;
        this.default_height = 100;
        this.show();

        this.leave_notify_event += on_leave_notify;
        
        do_menu();
	}
	
	private bool on_leave_notify(WorkspaceSelector wss, Gdk.EventCrossing event) {
		this.destroy();
		return false;
	}
	
	private void do_menu() {
		VBox rows = new VBox(true, 0);
		HBox row;
		
		for (int nrow=0; nrow<nrows; nrow++) {
			row = new HBox(true, 0);
			rows.pack_start(row, true, true, 0);
			for (int ncol=0; ncol<ncols; ncol++) {
				Button b = new Button.with_label("Desktop");
				b.width_request = b.height_request = 75;
				row.pack_start(b, true, true, 0);
			}
		}
		
		this.add(rows);
		this.show_all();
	}
	public int nrows {
		get {
			return target.get_workspace().get_height() / target.get_screen().get_height();
		}
	}
	public int ncols {
		get {
			return target.get_workspace().get_width() / target.get_screen().get_width();
		}
	}
}
