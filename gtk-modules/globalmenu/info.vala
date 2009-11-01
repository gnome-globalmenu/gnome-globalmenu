public class Info {
	public enum QuirkType {
		/* usual thing */
		NONE = 0,
		/* There is already a global menu on the window */
		NOT_A_GLOBAL = 1,
		/* for GtkMenuBar in a bonobo plug */
		HIDE_PARENT = 2,
		/* for wxGTK 2.8.10 programs */
		CHANGE_STYLE = 4,
	}
	public QuirkType quirks;

	public Gtk.MenuBar menubar;
	private bool dirty = false;
	public Gdk.Window event_window;

	public void queue_changed() {
		if(dirty == false) {
			dirty = true;
			Timeout.add(300, send_globalmenu_message);
		}
	}
	[CCode (cname = "gdk_window_set_menu_context")]
	protected extern void gdk_window_set_menu_context (Gdk.Window window, string? context);
	private bool send_globalmenu_message() {
		dirty = false;
		// send the message
		gdk_window_set_menu_context(event_window, 
				Serializer.to_string(menubar)
				);
		return true;
	}
}
