using GLib;
using DBus;
using Gtk;

[CCode (cprefix = "GMarkup", lower_case_cprefix = "g_markup_")]
namespace GMarkup {
	public class RemoteDocument: Document {
		private DocumentParser parser;
		private dynamic DBus.Object remote;
		private dynamic DBus.Connection conn;
		public bool invalid {get; set;}
		dynamic DBus.Object dbus;

		public string path {get; construct;}
		public string bus {get; construct;}
		private static HashTable<weak DocumentAddress, weak RemoteDocument> documents;

		private class DocumentAddress : GLib.Object {
			public string bus;
			public string path;
			public static bool equal(DocumentAddress addr1, DocumentAddress addr2) {
				return addr1.bus == addr2.bus && addr1.path == addr2.path;
			}
			public static uint hash(DocumentAddress addr) {
				return str_hash(addr.bus + addr.path);
			}
		}
		public static RemoteDocument connect(string bus, string path) {
			DocumentAddress addr = new DocumentAddress();
			addr.bus = bus;
			addr.path = path;
			if(documents == null) documents = new HashTable<weak DocumentAddress, weak RemoteDocument>.full((GLib.HashFunc)DocumentAddress.hash, (GLib.EqualFunc) DocumentAddress.equal, g_object_unref, g_object_unref);
			RemoteDocument document = documents.lookup(addr);
			if(document != null)
				return document;
			document = new RemoteDocument(bus, path);
			documents.insert(addr.ref() as DocumentAddress, document.ref() as RemoteDocument);
			return document;
		}	
		private override void dispose() {
			DocumentAddress addr = new DocumentAddress();
			addr.bus = bus;
			addr.path = path;
			documents.remove(addr);
		}
		private RemoteDocument(string bus, string path) {
			this.path = path;
			this.bus = bus;
		}
		construct {
			invalid = false;
			conn = Bus.get(DBus.BusType.SESSION);
			dbus = conn.get_object("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
			dbus.NameOwnerChanged += name_owner_changed;

			remote = conn.get_object(bus, path, "org.gnome.GlobalMenu.Document2");
			remote.Inserted += remote_inserted;
			remote.Removed += remote_removed;
			remote.Updated += remote_updated;
			parser = new DocumentParser(this);
			try {
				string meta_s = remote.QueryRoot();
				Node meta = parser.parse(meta_s);
				message("%s", meta.to_string());
				mergeMeta(meta, root, null);
			} catch (GLib.Error e) {
				warning("%s", e.message);
			}
		}
		private void remote_inserted(dynamic DBus.Object remote, int parent_id, int child_id, int refnode_id) {
			if(invalid) return;
			weak Node parent = getNode(parent_id);
			if(parent == null) return;
			debug("inserted to %s %d", parent.name, child_id);
			try {
				string s = remote.QueryNodeRecursively(child_id);
				weak Node refnode = getNode(refnode_id);
				mergeMeta(parser.parse(s), parent, refnode);
			} catch(GLib.Error e){
				warning("%s", e.message);
			}
		}
		private void remote_removed(dynamic DBus.Object remote, int parent_id, int child_id) {
			if(invalid) return;
			weak Node parent = getNode(parent_id);
			weak Node node = getNode(child_id);
			if(parent == null || node == null) return;
			parent.remove(node);
			this.destroyNode(node);
		}
		private void remote_updated(dynamic DBus.Object remote, int id, string? propname) {
			if(invalid) return;
			weak Node node = getNode(id);
			try {
				string s = remote.QueryNodeOnly(id);
				if(s == null) {
					warning("remote document didn't reply");
					return;
				}
				Node meta = parser.parse(s);
				mergeMeta(meta, root, null);
			} catch(GLib.Error e){
				warning("%s %s", e.domain.to_string(), e.message);
			}
		}
		private void name_owner_changed(dynamic DBus.Object object, string bus, string old_owner, string new_owner){
			if(bus != this.bus) return;
			if(new_owner != "" && old_owner == "") {
				message("new owner of %s", bus);
				invalid = false;
				return;
			}
			if(new_owner == "" && old_owner != "") {
				message("owner of %s disappeared", bus);
				List<weak Node> list = root.children.copy();
				foreach(weak Node child in list) {
					root.remove(child);
					this.destroyNode(child);
				}
				invalid = true;
				return;
			}
		}

		public static int test(string[] args) {
			Gtk.init(ref args);
			MainLoop loop = new MainLoop(null, false);
			RemoteDocument document = RemoteDocument.connect("org.gnome.GlobalMenu.DocumentTest", "/org/gnome/GlobalMenu/Document");
			ListView viewer = new ListView(document);
			Gtk.Window window = new Gtk.Window(Gtk.WindowType.TOPLEVEL);
			viewer.activated += (viewer, node) => {
				message("activated: %s", node.name);
			};
			window.add(viewer);
			window.show_all();
			loop.run();
			return 0;
		}
	}
}
