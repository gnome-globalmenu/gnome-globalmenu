using GLib;
using Gtk;
using DBus;
using GMarkup;

namespace Gnomenu {
	[DBus (name = "org.gnome.GlobalMenu.Server")]
	public class Server:DBusView {
		Connection conn;
		dynamic DBus.Object dbus;
		public Server(Document document) {
			this.document = document;
			this.path = "/org/gnome/GlobalMenu/Server";
		}
		construct {
			conn = Bus.get(DBus.BusType.SESSION);
			dbus = conn.get_object("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
			dbus.NameOwnerChanged += name_owner_changed;
			
			uint r = dbus.RequestName ("org.gnome.GlobalMenu.Server", (uint) 0);
			assert(r == DBus.RequestNameReply.PRIMARY_OWNER);
			debug("server ready");
		}
		private void name_owner_changed(dynamic DBus.Object object, string bus, string old_owner, string new_owner){
			if(new_owner != "") return;
			List<weak GMarkup.Node> to_remove;

			foreach (weak GMarkup.Node node in document.root.children) {
				if(node.get("bus") == bus) {
					to_remove.append(node);
				}
			}
			foreach(weak GMarkup.Node node in to_remove) {
				document.root.remove(node);
				document.destroyNode(node);
			}
		}
		private weak GMarkup.Node? find_node_by_xid(string xid) {
			return (document as Document).getNodeByName(xid);
		}
		public void RegisterWindow (string client_bus, string xid) {
			weak GMarkup.Node node = find_node_by_xid(xid);
			if(node != null) {
				if(node.get("bus") == client_bus) {
					return;
				} else {
					/*remove the old client that binds to the window*/
					document.root.remove(node);
				}
			}	
			node = document.createElement("client");
			node.freeze();
				node.set("name", xid);
				node.set("bus", client_bus);
			node.thaw();
			document.root.append(node);
			debug("register window %s %s", client_bus, xid);
		}
		public void RemoveWindow (string client_bus, string xid) {
			weak GMarkup.Node node = find_node_by_xid(xid);
			debug("remove window %s %s", client_bus, xid);
			if(node != null)
				if(node.get("bus") == client_bus) {
					document.root.remove(node);
					document.destroyNode(node);
				}
		}
		public void SetDefault (string xid) {
			foreach (weak GMarkup.Node node in document.root.children) {
					node.set("default", null);
			}
			weak GMarkup.Node node = find_node_by_xid(xid);
			node.set("default", "true");
		}
	}
}
