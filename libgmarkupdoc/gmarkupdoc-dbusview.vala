using GLib;
using Gdk;
using DBus;

namespace GMarkupDoc {
	[DBus (name = "org.gnome.GlobalMenu.Document")]
	public class DBusView:GLib.Object {
		Connection conn;
		dynamic DBus.Object dbus;
		[DBus (visible = false)]
		public string path {get; construct;}
		[DBus (visible = false)]
		public DocumentModel document {get; construct;}
		public DBusView(DocumentModel document, string object_path) {
			this.document = document;
			path = object_path;
		}
		construct {
			conn = Bus.get(DBus.BusType.SESSION);
			dbus = conn.get_object("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");

			conn.register_object(path, this);
			document.inserted += (document, parent, child, pos) => {
				inserted(parent.name, child.name, pos);
			};
			document.removed += (document, parent, child) => {
				removed(parent.name, child.name);
			};
			document.updated += (document, node, prop) => {
				updated(node.name, prop);
			};
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
				this.document.activate(node, 0);
			}
		}
		public signal void updated(string name, string prop);
		public signal void inserted(string parent, string name, int pos);
		public signal void removed(string parent, string name);

		public static int test(string[] args) {
			MainLoop loop = new MainLoop(null, false);
			Document document = new Document();
			GMarkupDoc.Parser parser = new GMarkupDoc.Parser(document);
			DBusView c = new DBusView(document, "/org/gnome/GlobalMenu/Document");
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
			loop.run();
			return 0;
		}
	}
}
