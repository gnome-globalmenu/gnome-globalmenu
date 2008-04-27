using GLib;

[DBus (name = "org.gnomenu.Menu")]
public class Menu: Object {
	private List<MenuItem> children_;
	public List<MenuItem> children {
		get {
			if(children_ == null) children_ = new List<MenuItem>();
			return children_;
		}
	}
	public Menu(){
	}
	public string ping() {
		stdout.printf("A Menu is pong");
		return "echo from Menu";
	}
}

[DBus (name = "org.gnomenu.MenuItem")]
public class MenuItem: Object {
	public Menu submenu {
		get; set;
	}
	public MenuItem(){
		submenu = null;
	}
	public signal void activated();
	public void activate(){
		activated();
	}
	public string ping() {
		message("A Menu Item is pong");
		return "echo from Menu";
	}
}
