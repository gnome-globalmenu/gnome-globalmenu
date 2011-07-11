[DBus (name="org.globalmenu.menu")]
public interface Menu: Object {
	public abstract void get_ui(string path, out string ui) throws IOError;
	public abstract uint64 xwindow {get; }
	public abstract void emit(string path) throws IOError;
}

private class KnownBusName {
	public Datalist<Menu> known_objects;
	public uint watch_id;
}

[DBus (name="org.globalmenu.manager")]
public class Manager: Object {
	public Datalist<KnownBusName?> known_bus_names;
	public void add(string object_path, BusName sender) {
		KnownBusName? k = known_bus_names.get_data(sender);
		if(k == null) {
			k = new KnownBusName();
			uint id = Bus.watch_name(BusType.SESSION, sender, BusNameWatcherFlags.NONE,
				() => {}, () => {
					print("client %s dead, reaping all menus\n", sender);
					Bus.unwatch_name(k.watch_id);
					known_bus_names.remove_no_notify(sender);
				});
			k.watch_id = id;
			known_bus_names.set_data(sender, k);
		}
		Timeout.add_seconds(0, ()=> {
			try {
				Menu menu = Bus.get_proxy_sync(BusType.SESSION, sender, object_path, DBusProxyFlags.DO_NOT_AUTO_START);
				k.known_objects.set_data(object_path, menu);
			} catch (IOError e) {
				warning("Manager can't access client %s\n", e.message);
			}
			return false;
			});
	}
	public void get_ui(uint64 xwindow, string path, out string ui) {
		Menu menu = get_menu_by_xwindow(xwindow);
		if(menu == null) {
			ui = "<empty/>";
			return;
		}
		menu.get_ui(path, out ui);
	}
	public void emit(uint64 xwindow, string path) {
		Menu menu = get_menu_by_xwindow(xwindow);
		if(menu == null) return;
		menu.emit(path);
	}
	public void enumerate(out string list) {
		StringBuilder sb = new StringBuilder("");
		known_bus_names.foreach(
			(bus_id, k) => {
				((KnownBusName)k).known_objects.foreach((object_id, menu) => {
					var bus_name =	bus_id.to_string();
					var object_path = object_id.to_string();
					Menu m = Bus.get_proxy_sync(BusType.SESSION, bus_name, object_path, DBusProxyFlags.DO_NOT_AUTO_START);
					sb.append_printf("%llu:%s:%s\n", m.xwindow, bus_name, object_path);
					}
				);
			}
		);

		list = sb.str;
	}
	private Menu get_menu_by_xwindow(uint64 xwindow) {
		Menu rt = null;
		known_bus_names.foreach(
			(key_id, k) => {
				((KnownBusName)k).known_objects.foreach((key_id, menu) => {
					uint64 hey = ((Menu)menu).xwindow;
					if (hey == xwindow) {
						rt = (Menu) menu;	
					}
					}
				);
			}
		);
		return rt;
	}
}

void main () {
	Manager man = new Manager();

	Bus.own_name(BusType.SESSION, "org.globalmenu.manager",
		BusNameOwnerFlags.NONE,
		(conn, name) => {
			try {
				conn.register_object("/org/globalmenu/manager", man);
			} catch (IOError e) {
				error("could not register service\n");
			}
		},
		() => {},
		() => error("could not aquire name\n"));

	new MainLoop().run();
	
}
