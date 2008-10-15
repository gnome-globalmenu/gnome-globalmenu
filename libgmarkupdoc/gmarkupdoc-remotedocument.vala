using GLib;
using DBus;
using Gtk;
namespace GMarkupDoc {
	public class RemoteDocument: Document {
		private Parser parser;
		private dynamic DBus.Object remote;
		private dynamic DBus.Connection conn;
		public bool invalid {get; set;}
		dynamic DBus.Object dbus;

		public string path {get; construct;}
		public string bus {get; construct;}
		
		public RemoteDocument(string bus, string path) {
			this.path = path;
			this.bus = bus;
		}
		construct {
			invalid = false;
			conn = Bus.get(DBus.BusType.SESSION);
			dbus = conn.get_object("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
			dbus.NameOwnerChanged += name_owner_changed;

			remote = conn.get_object(bus, path, "org.gnome.GlobalMenu.Document");
			remote.Inserted += remote_inserted;
			remote.Removed += remote_removed;
			remote.Updated += remote_updated;
			remote.Renamed += remote_renamed;
			parser = new Parser(this);
			try {
				string xml = remote.QueryRoot(-1);
				parser.parse(xml);
			} catch (GLib.Error e) {
				warning("%s", e.message);
			}
			this.activated += (document, node, detail) => {
				try {
					this.remote.Activate(node.name);
					assert(node == dict.lookup(node.name));
					debug("activated");
				} catch(GLib.Error e){
					warning("%s", e.message);
				}

			};
		}
		private void remote_inserted(dynamic DBus.Object remote, string type, string parentname, string nodename, int pos) {
			if(invalid) return;
			weak Node parent = dict.lookup(parentname);
			debug("inserted %s %s", parentname, nodename);
			if(parent == null) return;
			try {
				string s = remote.QueryNode(nodename, 0);
				debug("%s inserted", s);
				switch(type) {
					case "tag":
						parser.parse_child(parent, s, pos);
					break;
					case "text":
						Node text = (this as DocumentModel).CreateText(s);
						text.name = nodename;
						parent.insert(text, pos);
					break;
					case "special":
						Node special = (this as DocumentModel).CreateSpecial(s);
						special.name = nodename;
						parent.insert(special, pos);
					break;
				}
			} catch(GLib.Error e){
				warning("%s", e.message);
			}
		}
		private void remote_removed(dynamic DBus.Object remote, string parentname, string nodename) {
			if(invalid) return;
			weak Node parent = dict.lookup(parentname);
			weak Node node = dict.lookup(nodename);
			if(parent == null || node == null) return;
			parent.remove(node);
			this.DestroyNode(node);
		}
		private void remote_renamed(dynamic DBus.Object remote, string oldname, string newname) {
			if(invalid) return;
			debug("renamed %s to %s", oldname, newname);
			weak Node node = dict.lookup(oldname);
			if(node == null) return;
			node.name = newname;
		}
		private void remote_updated(dynamic DBus.Object remote, string nodename, string? propname) {
			if(invalid) return;
			weak Node node = dict.lookup(nodename);
			try {
				string s = remote.QueryNode(nodename, 0);
				if(s == null) {
					warning("remote document didn't reply");
					return;
				}
				if(node is Tag) parser.update_tag(node as Tag, propname, s);
				if(node is Text) (node as Text).text = s;
				if(node is Special) (node as Special).text = s;
			} catch(GLib.Error e){
				warning("%s %s", e.domain.to_string(), e.message);
			}
		}
		private void name_owner_changed(dynamic DBus.Object object, string bus, string old_owner, string new_owner){
			if(bus != this.bus) return;
			if(new_owner != "" && old_owner == "") {
				message("new owner of %s", bus);
				try {
					string xml = remote.QueryRoot(-1);
					parser.parse(xml);
				} catch (GLib.Error e) {
					warning("%s", e.message);
				}
				invalid = false;
				return;
			}
			if(new_owner == "" && old_owner != "") {
				message("owner of %s disappeared", bus);
				List<weak Node> list = root.children.copy();
				foreach(weak Node child in list) {
					root.remove(child);
					this.DestroyNode(child);
				}
				invalid = true;
				return;
			}
		}

		public static int test(string[] args) {
			Gtk.init(ref args);
			MainLoop loop = new MainLoop(null, false);
			RemoteDocument document = new RemoteDocument("org.gnome.GlobalMenu.DocumentTest", "/org/gnome/GlobalMenu/Document");
			ListView viewer = new ListView(document);
			Gtk.Window window = new Gtk.Window(Gtk.WindowType.TOPLEVEL);
			viewer.activated += (viewer, node, detail) => {
				viewer.document.activate(node, detail);
			};
			window.add(viewer);
			window.show_all();
			loop.run();
			return 0;
		}
	}
}
