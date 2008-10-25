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
				((GLibCompat.String)appname).canon("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_", '_');
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
		private void add_widget(string? parent, string name, int pos = -1) {
			weak GMarkup.Node node = document.dict.lookup(name);
			weak GMarkup.Node parent_node;
			if(parent == null) {
				parent_node = document.root;
			}
			else {
				parent_node = document.dict.lookup(parent);
			}
			if(node == null) {
				string[] names = {"name"};
				string[] values = {name};
				GMarkup.Tag node = document.CreateTagWithAttributes("widget", names, values);
				parent_node.insert(node, pos);
			}
		}
		private void remove_widget(string name) {
			weak GMarkup.Node node = document.dict.lookup(name);
			if(node != null) {
				assert(node.parent != null);
				node.parent.remove(node);
				node = null;
			}
		}
		[DBus (visible = false)]
		protected void register_window(string name, string xid) {
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
		protected void unregister_window(string name) {
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
		protected void set_default(string name) {
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
		protected void set_transient_for(string name, string parent_name) {
			weak GMarkup.Tag node = document.dict.lookup(name) as GMarkup.Tag;
			weak GMarkup.Tag parent_node = document.dict.lookup(parent_name) as GMarkup.Tag;
			if(node != null && parent_node != null) {
				weak string xid = node.get("xid");
				node.set("transient-for", parent_node.name);
				try {
					server.SetTransientFor(node.get("xid"), parent_node.get("xid"));
				} catch(GLib.Error e) {
					warning("%s", e.message);
				}
			}
		}
		public static int test(string[] args) {
			Gtk.init(ref args);
			MainLoop loop = new MainLoop(null, false);
			DocumentModel document = new Document();
			Client c = new Client(document);
			c.add_widget(null, "window1");
			c.add_widget(null, "window2");
			c.add_widget("window1", "menu1");
			c.add_widget("window2", "menu2");
			c.register_window("window1", "0000000");
			c.register_window("window2", "0000001");
//			c.remove_widget("window1");
			loop.run();
			return 0;
		}
	}
}
