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
		public void RegisterWindow (string client_bus, string window) {
			XMLTagNode node = new XMLTagNode(xml.S("client"));
			node.set(xml.S("bus"), client_bus);
			node.set(xml.S("window"), window);
			clients.children.append(node as XMLNode);
		}
		public string FindClient(string window) {
			foreach (weak XMLNode node in clients.children) {
				if(node is XMLTagNode) {
					weak XMLTagNode tagnode = node as XMLTagNode;
					if(tagnode.get("window") == window) {
						return tagnode.get("bus");
					}
				}
			}
			return "";
		}
		public void RemoveWindow (string client_bus, string window) {
			foreach (weak XMLNode node in clients.children) {
				if(node is XMLTagNode) {
					weak XMLTagNode tagnode = node as XMLTagNode;
					if(tagnode.get("window") == window &&
						tagnode.get("bus") == client_bus) {
						clients.children.remove(node);
						break;
					}
				}
			}
		}
		public string ListClients() {
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
