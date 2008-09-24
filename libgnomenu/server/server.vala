using GLib;
using Gtk;
using DBus;
using XML;
using Gnomenu;

[DBus (name = "org.gnome.GlobalMenu.Server")]
public class Server:GLib.Object {
	public class Document : XML.Document {
		public Document() { }
		public override XML.Document.Text CreateText(string text) {
			XML.Document.Text rt = new XML.Document.Text(this);
			rt.text = text;
			return rt;
		}
		public override  XML.Document.Special CreateSpecial(string text) {
			XML.Document.Special rt = new XML.Document.Special(this);
			rt.text = text;
			return rt;
		}
		public override XML.Document.Tag CreateTag(string tag) {
			XML.Document.Tag rt = new XML.Document.Tag(this);
			rt.tag = S(tag);
			return rt;
		}
		public override void FinishNode(XML.Node node) { }
	}

	Connection conn;
	Document document;
	weak XML.Node clients;
	dynamic DBus.Object dbus;
	public Server() {}
	construct {
		conn = Bus.get(DBus.BusType.SESSION);
		dbus = conn.get_object("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
		dbus.NameOwnerChanged += name_owner_changed;
		
		uint r = dbus.RequestName ("org.gnome.GlobalMenu.Server", (uint) 0);
		assert(r == DBus.RequestNameReply.PRIMARY_OWNER);
		conn.register_object("/org/gnome/GlobalMenu/Server", this);
		document = new Document();
		XML.Node clients = document.CreateTag("clients");
		document.root.append(clients);
		this.clients = clients;
		message("server ready");
	}
	private void name_owner_changed(dynamic DBus.Object object, string bus, string old_owner, string new_owner){
		if(new_owner != "") return;
		/*FIXME: this is buggy, node_it is freed before node_it->next is got*/
		foreach (weak XML.Node node in clients.children) {
			if(node is XML.Document.Tag) {
				weak XML.Document.Tag tagnode = node as XML.Document.Tag;
				if(tagnode.get("bus") == bus) {
					clients.remove(node);
					break;
				}
			}
		}
	}
	private weak XML.Document.Tag? find_node_by_xid(string xid) {
		foreach (weak XML.Node node in clients.children) {
			if(node is XML.Document.Tag) {
				weak XML.Document.Tag tagnode = node as XML.Document.Tag;
				if(tagnode.get("xid") == xid) {
					return tagnode;
				}
			}
		}
		return null;
	}
	public void RegisterWindow (string client_bus, string xid) {
		XML.Document.Tag node = find_node_by_xid(xid);
		if(node!=null) {
			if(node.get("bus") == client_bus) {
				return;
			} else {
				/*remove the old client that binds to the window*/
				clients.remove(node);
			}
		}	
		node = document.CreateTag("client");
		node.set("bus", client_bus);
		node.set("xid", xid);
		clients.append(node);
		message("register window %s %s", client_bus, xid);
		document.FinishNode(node);
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
				clients.remove(node);
	}
	public string QueryWindows() {
		return clients.to_string();
	}
}
