using GLib;
using DBus;
using Gtk;

[CCode (cprefix = "GMarkup", lower_case_cprefix = "g_markup_")]
namespace GMarkup {
	[DBus (name = "org.gnome.GlobalMenu.Document2")]
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
		private void document_inserted(DocumentModel document, Node parent, Node child, Node? refnode) {
			inserted(parent.id, child.id, (refnode!=null)?refnode.id:-1);
		}
		private void document_removed(DocumentModel document, Node parent, Node child) {
			removed(parent.id, child.id);
		}
		private void document_updated(DocumentModel document, Node node, string? prop) {
			updated(node.id, prop);
		}

		construct {
			conn = Bus.get(DBus.BusType.SESSION);
			dbus = conn.get_object("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");

			conn.register_object(path, this);
		}
		public string QueryRoot() {
			return document.root.to_meta_string();
		}
		public string QueryNodeOnly(int id){
			weak Node node = document.getNode(id);
			if(node!= null)
				return node.to_meta_string(false);
			return "";
		}
		public string QueryNodeRecursively(int id){
			weak Node node = document.getNode(id);
			if(node!= null)
				return node.to_meta_string(true);
			return "";
		}

		public signal void updated(int name, string? prop);
		public signal void inserted(int parent, int name, int refnode);
		public signal void removed(int parent, int name);

		public static int test(string[] args) {
			Gtk.init(ref args);
			MainLoop loop = new MainLoop(null, false);
			Document document = new Document();
			GMarkup.DocumentParser parser = new GMarkup.DocumentParser(document);
			DBusView c = new DBusView(document, "/org/gnome/GlobalMenu/Document");
			ListView l = new ListView(document);
			c.dbus.RequestName("org.gnome.GlobalMenu.DocumentTest", (uint) 0);
			document.root.append(parser.parse(
"""
<html><title>title</title>
<body name="body">
<div name="header">
	<h1> This is a header</h1>
</div>
<div name="content"></div>
<div name="tail"></div>
</body>
"""
));
			Gtk.Window window = new Gtk.Window(Gtk.WindowType.TOPLEVEL);
			window.add(l);
			window.show_all();
			loop.run();
			return 0;
		}
	}
}
