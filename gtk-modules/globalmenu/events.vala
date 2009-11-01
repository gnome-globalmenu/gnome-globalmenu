/* Changed queue */
public class Events {
	private bool dirty = false;
	private Gtk.MenuBar menubar;
	public void queue_changed(Gtk.MenuBar menubar) {
		if(dirty == false) {
			dirty = true;
			Timeout.add(300, send_globalmenu_message);
		}
	}
	private bool send_globalmenu_message() {
		dirty = false;
		// send the message
		return true;
	}
}
