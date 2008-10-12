using GLib;
using DBus;
using Gtk;

namespace GMarkupDoc {
	[DBus (name = "org.gnome.GlobalMenu.Document")]
	public class DBusView:GLib.Object, View {
		Connection conn;
		private DocumentModel _document;
		dynamic DBus.Object dbus;
		[DBus (visible = false)]
		public string path {get; construct;}
		[DBus (visible = false)]
		public weak DocumentModel? document {
			get {
				return _document;
			} 
			set {
				if(document != null) {
					document.inserted -= document_inserted;
					document.removed -= document_removed;
					document.updated -= document_updated;
				} 
				_document = value;
				if(document != null) {
					document.inserted += document_inserted;
					document.removed += document_removed;
					document.updated += document_updated;
				}
			}
		}
		public DBusView(DocumentModel document, string object_path) {
			this.document = document;
			path = object_path;
		}
		private void document_inserted(DocumentModel document, Node parent, Node child, int pos) {
			inserted(parent.name, child.name, pos);
		}
		private void document_removed(DocumentModel document, Node parent, Node child) {
			removed(parent.name, child.name);
		}
		private void document_updated(DocumentModel document, Node node, string prop) {
			updated(node.name, prop);
		}
		construct {
			conn = Bus.get(DBus.BusType.SESSION);
			dbus = conn.get_object("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");

			conn.register_object(path, this);
		}
		public string QueryRoot(int level = -1) {
			return document.root.summary(level);
		}
		public string QueryNode(string name, int level = -1){
			weak Node node = document.dict.lookup(name);
			if(node!= null)
				return node.summary(level);
			return "";
		}
		public void Activate(string name){
			weak Node node = document.dict.lookup(name);
			message("activated %s", name);
			if(node != null) {
				this.document.activated(node, 0);
				this.activated(node, 0);
			}
		}
		public signal void updated(string name, string prop);
		public signal void inserted(string parent, string name, int pos);
		public signal void removed(string parent, string name);

		public static int test(string[] args) {
			Gtk.init(ref args);
			MainLoop loop = new MainLoop(null, false);
			Document document = new Document();
			GMarkupDoc.Parser parser = new GMarkupDoc.Parser(document);
			DBusView c = new DBusView(document, "/org/gnome/GlobalMenu/Document");
			ListView l = new ListView(document);
			c.dbus.RequestName("org.gnome.GlobalMenu.DocumentTest", (uint) 0);
			parser.parse(
"""
<html><title>title</title>
<body name="body">
<div name="header">
	<h1> This is a header</h1>
</div>
<div name="content"></div>
<div name="tail"></div>
</body>
""");
			Gtk.Window window = new Gtk.Window(Gtk.WindowType.TOPLEVEL);
			window.add(l);
			window.show_all();
			loop.run();
			return 0;
		}
	}
}
