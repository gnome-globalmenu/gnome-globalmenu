using GLib;
using Gtk;
using DBus;
using Markup;
namespace Gnomenu {

	[DBus (name = "org.gnome.GlobalMenu.Server")]
	public class Server:GLib.Object {
		Connection conn;
		XML xml;
		XMLNode clients;
		dynamic DBus.Object dbus;
		public Server() {}
		private void name_owner_changed(dynamic DBus.Object object, string bus, string old_owner, string new_owner){
			if(new_owner != "") return;
			/*FIXME: this is buggy, node_it is freed before node_it->next is got*/
			foreach (weak XMLNode node in clients.children) {
				if(node is XMLTagNode) {
					weak XMLTagNode tagnode = node as XMLTagNode;
					if(tagnode.get("bus") == bus) {
						clients.children.remove(node);
						break;
					}
				}
			}
			
		}
		construct {
			conn = Bus.get(DBus.BusType.SESSION);
			dbus = conn.get_object("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
			dbus.NameOwnerChanged += name_owner_changed;
			
			uint r = dbus.RequestName ("org.gnome.GlobalMenu.Server", (uint) 0);
			assert(r == DBus.RequestNameReply.PRIMARY_OWNER);
			conn.register_object("/org/gnome/GlobalMenu/Server", this);
			xml = new XML();
			clients = new XMLTagNode(xml.S("clients"));
			xml.root.children.append(clients);
		}
		private weak XMLTagNode? find_node_by_xid(string xid) {
			foreach (weak XMLNode node in clients.children) {
				if(node is XMLTagNode) {
					weak XMLTagNode tagnode = node as XMLTagNode;
					if(tagnode.get("xid") == xid) {
						return tagnode;
					}
				}
			}
			return null;
		}
		public void RegisterWindow (string client_bus, string xid) {
			XMLTagNode node = find_node_by_xid(xid);
			if(node!=null) {
				if(node.get("bus") == client_bus) {
					return;
				} else {
					/*remove the old client that binds to the window*/
					clients.children.remove(node);
				}
			}	
			node =new XMLTagNode(xml.S("client"));
			node.set(xml.S("bus"), client_bus);
			node.set(xml.S("xid"), xid);
			clients.children.append(node as XMLNode);
		}
		public string QueryWindow(string xid) {
			XMLTagNode node = find_node_by_xid(xid);
			if(node != null) return node.to_string();
			return "";
		}
		public void RemoveWindow (string client_bus, string xid) {
			XMLTagNode node= find_node_by_xid(xid);
			if(node != null)
				if(node.get("bus") == client_bus)
					clients.children.remove(node);
		}
		public string QueryWindows() {
			return xml.root.to_string();
		}
		public static int test(string[] args) {
			Gtk.init(ref args);
			MainLoop loop = new MainLoop(null, false);
			Server c = new Server();
			loop.run();
			return 0;
		}
	}
}
