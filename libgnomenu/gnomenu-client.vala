using GLib;
using Gdk;
using Gtk;
using DBus;
using GMarkup;
namespace Gnomenu {
	[DBus (name = "org.gnome.GlobalMenu.Client")]
	public class Client:DBusView {
		Connection conn;
		public string bus;
		dynamic DBus.Object dbus;
		dynamic DBus.Object server;
		public Client(DocumentModel document) {
			this.document = document;
			path = "/org/gnome/GlobalMenu/Application";
		}
		construct {
			conn = Bus.get(DBus.BusType.SESSION);
			dbus = conn.get_object("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
			server = conn.get_object("org.gnome.GlobalMenu.Server", "/org/gnome/GlobalMenu/Server", "org.gnome.GlobalMenu.Server");
			
			Rand rand = new Rand();
			uint r;
			do {
				string str = rand.next_int().to_string().strip();
				string appname = Environment.get_prgname();
				appname.canon("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_", '_');
				if(appname[0] >= '0' && appname[0] <='9') appname = "_"+appname;
				bus = "org.gnome.GlobalMenu.Applications." + appname + "-" + str;
				debug("Obtaining BUS name: %s", bus);
				r = dbus.RequestName (bus, (uint) 0);
			} while(r != DBus.RequestNameReply.PRIMARY_OWNER);
		}
		public string QueryXID(string xid) {
			weak GMarkup.Node node = find_window_by_xid(xid);
			if(node != null) {
				return node.name;
			}
			return "";
		}
		public void Activate(string xid, string nodename) {
			weak GMarkup.Node window = find_window_by_xid(xid);
			weak GMarkup.Node node = this.document.dict.lookup(nodename);
			if(node == null) warning("node: %s not found", nodename);
			this.activated(window, node);
		}
		[DBus (visible = false)]
		public signal void activated(GMarkup.Node? window, GMarkup.Node? node);

		private weak GMarkup.Node? find_window_by_xid(string xid) {
			foreach (weak GMarkup.Node node in document.root.children) {
				if(node is GMarkup.Tag) {
					weak GMarkup.Tag tag = node as GMarkup.Tag;
					if(tag.get("xid") == xid) {
						return tag;
					}
				}
			}
			return null;
		}
		[DBus (visible = false)]
		public void register_window(string name, string xid) {
			weak GMarkup.Tag node = document.dict.lookup(name) as GMarkup.Tag;
			if(node != null) {
				node.set("xid", xid);
				try {
					server.RegisterWindow(this.bus, xid);
				} catch(GLib.Error e) {
					warning("%s", e.message);
				}
			}
		}
		[DBus (visible = false)]
		public void unregister_window(string name) {
			weak GMarkup.Tag node = document.dict.lookup(name) as GMarkup.Tag;
			if(node != null) {
				weak string xid = node.get("xid");
				try {
					server.RemoveWindow(this.bus, xid);
				} catch(GLib.Error e) {
					warning("%s", e.message);
				}
				node.unset("xid");
			}

		}
		[DBus (visible = false)]
		public void set_default(string name) {
			weak GMarkup.Tag node = document.dict.lookup(name) as GMarkup.Tag;
			if(node != null) {
				weak string xid = node.get("xid");
				try {
					server.SetDefault(node.get("xid"));
				} catch(GLib.Error e) {
					warning("%s", e.message);
				}
			}
		}
		[DBus (visible = false)]
		public void attach_menu_bar(string windowname, string menubarname) {
			weak GMarkup.Tag node = document.dict.lookup(windowname) as GMarkup.Tag;
			GMarkup.Tag ref_node = document.CreateTag("ref");
			ref_node.set("target", menubarname);
			warning("%u", ref_node.ref_count);
			node.append(ref_node);
		}
		[DBus (visible = false)]
		public void detach_menu_bar(string windowname, string menubarname) {
			weak GMarkup.Tag node = document.dict.lookup(windowname) as GMarkup.Tag;
			foreach(weak GMarkup.Node child in node.children) {
				if(child is GMarkup.Tag)
					if((child as GMarkup.Tag).get("target") == menubarname) {
						node.remove(child);
						break;
					}
			}
		}
		public static int test(string[] args) {
			Gtk.init(ref args);
			MainLoop loop = new MainLoop(null, false);
			DocumentModel document = new Document();
			Client c = new Client(document);
			DocumentParser parser = new DocumentParser(document);
			parser.parse(
"""
		<window name="window1"/>
		<window name="window2"/>
		<menubar name="menubar">
			<item name="file" label="file"/>
		</menubar>
""");
			c.attach_menu_bar("window1", "menubar");
			c.attach_menu_bar("window2", "menubar");
			c.detach_menu_bar("window1", "menubar");
			c.register_window("window1", "0000000");
			c.register_window("window2", "0000001");
			loop.run();
			return 0;
		}
	}
}
