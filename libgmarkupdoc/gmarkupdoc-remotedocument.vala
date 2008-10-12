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
					debug("activated");
				} catch(GLib.Error e){
					warning("%s", e.message);
				}

			};
		}
		private void remote_inserted(dynamic DBus.Object remote, string parentname, string nodename, int pos) {
			if(invalid) return;
			weak Node parent = dict.lookup(parentname);
			if(parent == null) return;
			try {
				string s = remote.QueryNode(nodename, 0);
				if(s == null) {
					warning("remote document didn't reply");
					return;
				}
				parser.parse_child(parent, s, pos);
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
		}
		private void remote_updated(dynamic DBus.Object remote, string nodename, string propname) {
			if(invalid) return;
			weak Tag node = dict.lookup(nodename) as Tag;
			try {
				string s = remote.QueryNode(nodename, 0);
				if(s == null) {
					warning("remote document didn't reply");
					return;
				}
				parser.update_tag(node, propname, s);
			} catch(GLib.Error e){
				warning("%s %s", e.domain.to_string(), e.message);
			}
		}
		private void name_owner_changed(dynamic DBus.Object object, string bus, string old_owner, string new_owner){
			if(bus != this.bus) return;
			if(new_owner != "" && old_owner == "") {
				message("new owner of %s", bus);
				this.root.remove_all();
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
				this.root.remove_all();
				invalid = true;
				return;
			}
		}

		public static int test(string[] args) {
			Gtk.init(ref args);
			MainLoop loop = new MainLoop(null, false);
			RemoteDocument document = new RemoteDocument("org.gnome.GlobalMenu.Server", "/org/gnome/GlobalMenu/Server");
			ListView viewer = new ListView(document);
			Gtk.Window window = new Gtk.Window(Gtk.WindowType.TOPLEVEL);
			window.add(viewer);
			window.show_all();
			loop.run();
			return 0;
		}
	}
}
