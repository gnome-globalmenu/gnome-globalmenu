using GLib;
using Gdk;
using Gtk;
using DBus;
using XML;
namespace Gnomenu {
	[DBus (name = "org.gnome.GlobalMenu.Client")]
	public class Client:GLib.Object {
		public abstract class NodeFactory: XML.NodeFactory {
			public override RootNode CreateRootNode() {
				RootNode rt = new RootNode(this);
				rt.freeze();
				return rt;
			}
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
			public abstract virtual weak XML.TagNode? lookup(string name);
			public abstract virtual TagNode CreateWidgetNode(string name);
			public override void FinishNode(XML.Node node) {
				node.unfreeze();
			}
		}
		Connection conn;
		string bus;
		dynamic DBus.Object dbus;
		[DBus (visible = false)]
		public NodeFactory factory {get; construct;}
		protected TagNode windows;
		public Client(NodeFactory factory) {
			this.factory = factory;
		}
		construct {
			conn = Bus.get(DBus.BusType.SESSION);
			dbus = conn.get_object("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
			
			string str = dbus.GetId();
			bus = "org.gnome.GlobalMenu.Applications." + str;
			uint r = dbus.RequestName (bus, (uint) 0);
			assert(r == DBus.RequestNameReply.PRIMARY_OWNER);
			conn.register_object("/org/gnome/GlobalMenu/Application", this);
			windows = factory.CreateTagNode("windows");
			factory.FinishNode(windows);
			factory.added += (f, p, o, i) => {
				if(!(p is TagNode) || !(o is TagNode)) return;
				weak TagNode parent_node = p as TagNode;
				weak TagNode node = o as TagNode;
				message("added %s to %s at %d", node.get("name"), parent_node.get("name"), i);
			};
			factory.removed += (f, p, o) => {
				if(!(p is TagNode) || !(o is TagNode)) return;
				weak TagNode parent_node = p as TagNode;
				weak TagNode node = o as TagNode;
				message("removed %s to %s", node.get("name"), parent_node.get("name"));
			};
			factory.updated += (f, o, prop) => {
				if(!(o is TagNode)) return;
				weak TagNode node = o as TagNode;
				message("updated %s of %s", prop, node.get("name"));
			};
		}
		public string QueryNode(string name, int level = -1){
			weak TagNode node = factory.lookup(name);
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
			weak TagNode node = factory.lookup(name);
			activate_item(node);
		}
		public signal void updated(string name);
		public signal void inserted(string parent, string name, int pos);
		public signal void removed(string parent, string name);

		protected dynamic DBus.Object get_server(){
			return conn.get_object("org.gnome.GlobalMenu.Server", "/org/gnome/GlobalMenu/Server", "org.gnome.GlobalMenu.Server");
		}
		protected virtual void activate_item(TagNode item_node) {
			message("%s is activated", item_node.summary(0));
		}
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
			weak TagNode node = factory.lookup(name);
			weak TagNode parent_node;
			if(parent == null) {
				parent_node = windows;
			}
			else
				parent_node = factory.lookup(parent);
			if(node == null) {
				TagNode node = factory.CreateWidgetNode(name);
				parent_node.insert(node, pos);
				factory.FinishNode(node);
			}
		}
		protected void remove_widget(string name) {
			weak TagNode node = factory.lookup(name);
			if(node != null) {
				assert(node.parent != null);
				node.parent.remove(node);
				node = null;
			}
		}
		protected void register_window(string name, string xid) {
			weak TagNode node = factory.lookup(name);
			if(node != null) {
				node.set("xid", xid);
				try {
					get_server().RegisterWindow(this.bus, xid);
				} catch(GLib.Error e) {
					warning("%s", e.message);
				}
			}
		}
		protected void unregister_window(string name) {
			weak TagNode node = factory.lookup(name);
			if(node != null) {
				weak string xid = node.get("xid");
				try {
					get_server().RemoveWindow(this.bus, xid);
				} catch(GLib.Error e) {
					warning("%s", e.message);
				}
				node.unset("xid");
			}

		}
		private class TestFactory: NodeFactory {
			private HashTable <weak string, weak XML.TagNode> dict;
			private class WidgetNode:TagNode {
				public WidgetNode(NodeFactory factory) {
					this.factory = factory;
				}
				~WidgetNode() {
					(this.factory as TestFactory).dict.remove(this.get("name"));
				}
			}
			public TestFactory() { }
			construct {
				dict = new HashTable<weak string, weak XML.TagNode>(str_hash, str_equal);
			}
			public override weak XML.TagNode? lookup(string name){
				return dict.lookup(name);
			}
			public override TagNode CreateWidgetNode(string name) {
				TagNode rt = new WidgetNode(this);
				rt.tag = S("widget");
				rt.set("name", name);
				dict.insert(name, rt);
				return rt;
			}
			public override void FinishNode(XML.Node node){}
		}
		public static int test(string[] args) {
			Gtk.init(ref args);
			MainLoop loop = new MainLoop(null, false);
			NodeFactory factory = new TestFactory();
			Client c = new Client(factory);
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
