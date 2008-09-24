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
		public weak TagNode windows;
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
			TagNode windows_ = document.CreateTagNode("windows");
			document.root.append(windows_);
			document.FinishNode(windows_);
			windows = windows_;
			document.added += (f, p, o, i) => {
				if(!(p is TagNode) || !(o is TagNode)) return;
				weak TagNode parent_node = p as TagNode;
				weak TagNode node = o as TagNode;
				message("added %s to %s at %d", node.get("name"), parent_node.get("name"), i);
			};
			document.removed += (f, p, o) => {
				if(!(p is TagNode) || !(o is TagNode)) return;
				weak TagNode parent_node = p as TagNode;
				weak TagNode node = o as TagNode;
				message("removed %s to %s", node.get("name"), parent_node.get("name"));
			};
			document.updated += (f, o, prop) => {
				if(!(o is TagNode)) return;
				weak TagNode node = o as TagNode;
				message("updated %s of %s", prop, node.get("name"));
			};
		}
		public string QueryNode(string name, int level = -1){
			weak TagNode node = document.lookup(name);
			if(node!= null)
				return node.summary(level);
			return "";
		}
		public string QueryXID(string xid) {
			weak TagNode node = find_window_by_xid(xid);
			if(node != null) {
				return node.summary(0);
			}
			return "";
		}
		public string QueryWindows() {
			return windows.summary(1);
		}
		public void ActivateItem(string name){
			weak Document.Widget node = document.lookup(name);
			node.activate();
		}
		public signal void updated(string name);
		public signal void inserted(string parent, string name, int pos);
		public signal void removed(string parent, string name);

		private weak TagNode? find_window_by_xid(string xid) {
			foreach (weak XML.Node node in windows.children) {
				if(node is TagNode) {
					weak TagNode tagnode = node as TagNode;
					if(tagnode.get("xid") == xid) {
						return tagnode;
					}
				}
			}
			return null;
		}
		protected void add_widget(string? parent, string name, int pos = -1) {
			weak TagNode node = document.lookup(name);
			weak TagNode parent_node;
			if(parent == null) {
				parent_node = windows;
			}
			else
				parent_node = document.lookup(parent);
			if(node == null) {
				TagNode node = document.CreateWidgetNode(name);
				parent_node.insert(node, pos);
				document.FinishNode(node);
			}
		}
		protected void remove_widget(string name) {
			weak Document.Widget node = document.lookup(name);
			if(node != null) {
				assert(node.parent != null);
				node.parent.remove(node);
				node = null;
			}
		}
		protected void register_window(string name, string xid) {
			weak TagNode node = document.lookup(name);
			if(node != null) {
				node.set("xid", xid);
				try {
					server.RegisterWindow(this.bus, xid);
				} catch(GLib.Error e) {
					warning("%s", e.message);
				}
			}
		}
		protected void unregister_window(string name) {
			weak TagNode node = document.lookup(name);
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
		private class TestFactory: Document {
			private class Widget:Document.Widget {
				public Widget(Document document) {
					this.document = document;
				}
				public override void activate() {
					message("%s is activated", summary(0));
				}
			}
			public TestFactory() { }
			public override TextNode CreateTextNode(string text) {
				TextNode rt = new TextNode(this);
				rt.freeze();
				rt.text = text;
				return rt;
			}
			public override  SpecialNode CreateSpecialNode(string text) {
				SpecialNode rt = new SpecialNode(this);
				rt.freeze();
				rt.text = text;
				return rt;
			}
			public override TagNode CreateTagNode(string tag) {
				TagNode rt = new TagNode(this);
				rt.freeze();
				rt.tag = S(tag);
				return rt;
			}
			public override Document.Widget CreateWidgetNode(string name) {
				Widget rt = new Widget(this);
				rt.tag = S("widget");
				rt.name = name;
				return rt;
			}
			public override void FinishNode(XML.Node node){}
		}
		public static int test(string[] args) {
			Gtk.init(ref args);
			MainLoop loop = new MainLoop(null, false);
			Document document = new TestFactory();
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
