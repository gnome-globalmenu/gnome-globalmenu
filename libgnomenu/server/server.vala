using GLib;
using Gtk;
using DBus;
using XML;
using Gnomenu;

[DBus (name = "org.gnome.GlobalMenu.Server")]
public class Server:GLib.Object {
	Connection conn;
	public Gnomenu.Document document {get; construct;}
	dynamic DBus.Object dbus;
	public Server(Gnomenu.Document document) {
		this.document = document;
	}
	construct {
		conn = Bus.get(DBus.BusType.SESSION);
		dbus = conn.get_object("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
		dbus.NameOwnerChanged += name_owner_changed;
		
		uint r = dbus.RequestName ("org.gnome.GlobalMenu.Server", (uint) 0);
		assert(r == DBus.RequestNameReply.PRIMARY_OWNER);
		conn.register_object("/org/gnome/GlobalMenu/Server", this);
		message("server ready");
	}
	private void name_owner_changed(dynamic DBus.Object object, string bus, string old_owner, string new_owner){
		if(new_owner != "") return;
		List<weak XML.Node> to_remove;

		foreach (weak XML.Node node in document.root.children) {
			if(node is XML.Document.Tag) {
				weak XML.Document.Tag tagnode = node as XML.Document.Tag;
				if(tagnode.get("bus") == bus) {
					to_remove.append(tagnode);
				}
			}
		}
		foreach(weak XML.Node node in to_remove) {
			document.root.remove(node);
		}
	}
	private weak XML.Document.Tag? find_node_by_xid(string xid) {
		return document.lookup(xid);
	}
	public void RegisterWindow (string client_bus, string xid) {
		XML.Document.Tag node = find_node_by_xid(xid);
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
		node.unfreeze();
		document.root.append(node);
		node.set("bus", client_bus);
		message("register window %s %s", client_bus, xid);
	}
	public string QueryWindow(string xid) {
		XML.Document.Tag node = find_node_by_xid(xid);
		if(node != null) return node.to_string();
		return "";
	}
	public void RemoveWindow (string client_bus, string xid) {
		XML.Document.Tag node= find_node_by_xid(xid);
		message("remove window %s %s", client_bus, xid);
		if(node != null)
			if(node.get("bus") == client_bus)
				document.root.remove(node);
	}
	public string QueryWindows() {
		return document.root.to_string();
	}
}
