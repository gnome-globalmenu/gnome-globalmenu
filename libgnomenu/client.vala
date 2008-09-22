using GLib;
using Gdk;
using Gtk;
using DBus;
using Markup;
namespace Gnomenu {
	[DBus (name = "org.gnome.GlobalMenu.Client")]
	public abstract class Client:GLib.Object {
		public class XMLWidgetNode: XMLTagNode {
			public HashTable<weak string, weak XMLWidgetNode> dict {get; construct;}
			public weak string widget {
				get {
					return get("widget");
				}
				set {
					if(value == null) remove("widget");
					set("widget", value);
				}
			}
			public XMLWidgetNode(string tag, string widget, HashTable<weak string, weak XMLWidgetNode> dict) {
				this.dict = dict;
				this.tag = tag;
				this.widget = widget;
				dict.insert(widget, this);
			}
			public weak XMLWidgetNode? lookup(string widget) {
				return dict.lookup(widget);
			}
			~XMLWidgetNode() {
				/* FIXME: vala doesn't invoke the deconstructor*/
				dict.remove(widget);
			}
		}
		Connection conn;
		string bus;
		dynamic DBus.Object dbus;
		protected XML xml;
		protected XMLNode windows;
		protected HashTable<weak string, weak XMLWidgetNode> dict;
		public Client() {
		}
		construct {
			conn = Bus.get(DBus.BusType.SESSION);
			dbus = conn.get_object("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
			
			string str = dbus.GetId();
			bus = "org.gnome.GlobalMenu.Applications." + str;
			uint r = dbus.RequestName (bus, (uint) 0);
			assert(r == DBus.RequestNameReply.PRIMARY_OWNER);
			conn.register_object("/org/gnome/GlobalMenu/Application", this);
			xml = new XML();
			windows = new XMLTagNode(xml.S("windows"));
			xml.root.children.append(windows);
			dict = new HashTable<weak string, weak XMLWidgetNode>(str_hash, str_equal);
		}
		public string QueryNode(string widget){
			weak XMLNode node = find_node_by_widget(widget);
			if(node!= null)
				return node.to_string();
			return "";
		}
		public string QueryXID(string xid) {
			weak XMLNode node = find_window_by_xid(xid);
			if(node != null) {
				return node.to_string();
			}
			return "";
		}
		public string QueryWindows() {
			return xml.root.to_string();
		}
		public void ActivateItem(string widget){
			weak XMLWidgetNode node = find_node_by_widget(widget);
			activate_item(node);
		}
		public signal void updated(string widget);

		protected dynamic DBus.Object get_server(){
			return conn.get_object("org.gnome.GlobalMenu.Server", "/org/gnome/GlobalMenu/Server", "org.gnome.GlobalMenu.Server");
		}
		protected abstract virtual void activate_item(XMLWidgetNode item_node);
		protected abstract virtual void sync_node(XMLWidgetNode node);
		private weak XMLWidgetNode? find_node_by_widget(string widget) {
			return dict.lookup(widget);
		}
		private weak XMLWidgetNode? find_window_by_xid(string xid) {
			foreach (weak XMLNode node in windows.children) {
				if(node is XMLWidgetNode) {
					weak XMLWidgetNode tagnode = node as XMLWidgetNode;
					if(tagnode.get("xid") == xid) {
						return tagnode;
					}
				}
			}
			return null;
		}
		protected void add_window(string widget) {
			weak XMLTagNode node = find_node_by_widget(widget);
			if(node == null) {
				message("add window %s stage 1", widget);
				XMLTagNode node = new XMLWidgetNode(
					xml.S("window"),
					xml.S(widget), dict);
				windows.children.append(node);
				message("add window %s stage 2", widget);
			}
		}
		protected void register_window(string widget, string xid) {
			weak XMLTagNode node = find_node_by_widget(widget);
			if(node != null) {
				node.set(xml.S("xid"), xid);
				try {
					get_server().RegisterWindow(this.bus, xid);
				} catch(GLib.Error e) {
					warning("%s", e.message);
				}
			}
		}
		protected void unregister_window(string widget) {
			weak XMLTagNode node = find_node_by_widget(widget);
			if(node != null) {
				weak string xid = node.get("xid");
				try {
					get_server().RemoveWindow(this.bus, xid);
				} catch(GLib.Error e) {
					warning("%s", e.message);
				}
				node.remove("xid");
			}

		}
		protected void remove_window(string widget) {
			weak XMLTagNode node = find_node_by_widget(widget);
			if(node != null) {
				windows.children.remove(node);
			}
		}
		protected void add_menu(string window_widget, string menu_widget) {
			weak XMLTagNode node = find_node_by_widget(window_widget);
			if(node == null) {
				warning("window %s doesn't exist", window_widget);
				return;
			}
			weak XMLTagNode menu_node = find_node_by_widget(menu_widget);
			if(menu_node != null) {
				warning("menu already added to the window");
				return;
			} else {
				XMLWidgetNode menu_node = new XMLWidgetNode(
							xml.S("menu"),
							xml.S(menu_widget), 
							dict);
				node.children.append(menu_node);
				sync_node(menu_node);
			}
		}
		protected void remove_menu(string window_widget, string menu_widget) {
			weak XMLTagNode node = find_node_by_widget(window_widget);
			if(node == null) {
				warning("window %s doesn't exist", window_widget);
				return;
			}
			weak XMLTagNode menu_node = find_node_by_widget(menu_widget);
			if(menu_node != null) {
				node.children.remove(menu_node);
			}
		}
	}
}
