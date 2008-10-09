using GLib;
using Gtk;
using DBus;
using GMarkupDoc;

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
			message("server ready");
		}
		private void name_owner_changed(dynamic DBus.Object object, string bus, string old_owner, string new_owner){
			if(new_owner != "") return;
			List<weak GMarkupDoc.Node> to_remove;

			foreach (weak GMarkupDoc.Node node in document.root.children) {
				if(node is GMarkupDoc.Tag) {
					weak GMarkupDoc.Tag tagnode = node as GMarkupDoc.Tag;
					if(tagnode.get("bus") == bus) {
						to_remove.append(tagnode);
					}
				}
			}
			foreach(weak GMarkupDoc.Node node in to_remove) {
				document.root.remove(node);
			}
		}
		private weak GMarkupDoc.Tag? find_node_by_xid(string xid) {
			return document.lookup(xid) as GMarkupDoc.Tag;
		}
		public void RegisterWindow (string client_bus, string xid) {
			GMarkupDoc.Tag node = find_node_by_xid(xid);
			if(node!=null) {
				if(node.get("bus") == client_bus) {
					return;
				} else {
					/*remove the old client that binds to the window*/
					document.root.remove(node);
				}
			}	
			node = document.CreateTag("client");
			node.freeze();
			node.set("name", xid);
			node.set("bus", client_bus);
			node.unfreeze();
			document.root.append(node);
			message("register window %s %s", client_bus, xid);
		}
		public void RemoveWindow (string client_bus, string xid) {
			GMarkupDoc.Tag node= find_node_by_xid(xid);
			message("remove window %s %s", client_bus, xid);
			if(node != null)
				if(node.get("bus") == client_bus)
					document.root.remove(node);
		}
	}
}
