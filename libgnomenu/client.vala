using GLib;
using Gdk;
using Gtk;
using DBus;
using XML;
namespace Gnomenu {
	[DBus (name = "org.gnome.GlobalMenu.Client")]
	public class Client:GLib.Object {
		Connection conn;
		public string bus;
		dynamic DBus.Object dbus;
		dynamic DBus.Object server;
		[DBus (visible = false)]
		public Document document {get; construct;}
		public Client(Document document) {
			this.document = document;
		}
		construct {
			conn = Bus.get(DBus.BusType.SESSION);
			dbus = conn.get_object("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
			server = conn.get_object("org.gnome.GlobalMenu.Server", "/org/gnome/GlobalMenu/Server", "org.gnome.GlobalMenu.Server");
			
			string str = dbus.GetId();
			bus = "org.gnome.GlobalMenu.Applications." + Environment.get_prgname() + str;
			message("Obtaining BUS name: %s", bus);
			uint r = dbus.RequestName (bus, (uint) 0);
			assert(r == DBus.RequestNameReply.PRIMARY_OWNER);
			conn.register_object("/org/gnome/GlobalMenu/Application", this);
			document.added += (f, p, o, i) => {
				if(!(p is Document.Widget) || !(o is Document.Widget)) return;
				weak Document.Widget parent_node = p as Document.Widget;
				weak Document.Widget node = o as Document.Widget;
				inserted(parent_node.name, node.name, i);
			};
			document.removed += (f, p, o) => {
				if(!(p is Document.Widget) || !(o is Document.Widget)) return;
				weak Document.Widget parent_node = p as Document.Widget;
				weak Document.Widget node = o as Document.Widget;
				removed(parent_node.name, node.name);
			};
			document.updated += (f, o, prop) => {
				if(!(o is Document.Widget)) return;
				weak Document.Widget node = o as Document.Widget;
				updated(node.name);
			};
		}
		public string QueryNode(string name, int level = -1){
			weak Document.Widget node = document.lookup(name);
			if(node!= null)
				return node.summary(level);
			return "";
		}
		public string QueryXID(string xid) {
			weak Document.Widget node = find_window_by_xid(xid);
			if(node != null) {
				return node.summary(0);
			}
			return "";
		}
		public string QueryWindows() {
			return( document.root.summary(1));
		}
		public void ActivateItem(string name){
			weak Document.Widget node = document.lookup(name);
			node.activate();
		}
		public signal void updated(string name);
		public signal void inserted(string parent, string name, int pos);
		public signal void removed(string parent, string name);

		private weak Document.Widget? find_window_by_xid(string xid) {
			foreach (weak XML.Node node in document.root.children) {
				if(node is Document.Widget) {
					weak Document.Widget widget = node as Document.Widget;
					if(widget.get("xid") == xid) {
						return widget;
					}
				}
			}
			return null;
		}
		private void add_widget(string? parent, string name, int pos = -1) {
			weak XML.Node node = document.lookup(name);
			weak XML.Node parent_node;
			if(parent == null) {
				parent_node = document.root;
			}
			else {
				parent_node = document.lookup(parent);
			}
			if(node == null) {
				string[] names = {"name"};
				string[] values = {name};
				XML.Document.Tag node = document.CreateTagWithAttributes("widget", names, values);
				parent_node.insert(node, pos);
			}
		}
		private void remove_widget(string name) {
			weak Document.Widget node = document.lookup(name);
			if(node != null) {
				assert(node.parent != null);
				node.parent.remove(node);
				node = null;
			}
		}
		[DBus (visible = false)]
		protected void register_window(string name, string xid) {
			weak Document.Widget node = document.lookup(name);
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
			weak Document.Widget node = document.lookup(name);
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
		public static int test(string[] args) {
			Gtk.init(ref args);
			MainLoop loop = new MainLoop(null, false);
			Document document = new Document();
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
