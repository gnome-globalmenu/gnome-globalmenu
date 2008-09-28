using GLib;
using Gtk;
using Gnomenu;
using GtkAQD;
using XML;

namespace Gnomenu {
	public class MenuView : GtkAQD.MenuBar {
		private Document _document;
		public weak Document? document {
			get {
				return _document;
			} set {
				_document = value;
			}
		}
		public MenuView(Document? document) {
			this.document = document;
			this.local = true;
		}
		private Gtk.Widget create_widget(Document.Widget widget) {
			Gtk.Widget rt;
			switch(widget.tag) {
				case "menu":
					Gtk.MenuShell gtk = new Gtk.Menu();
					message("adding a menu");
					foreach(weak XML.Node child in widget.children) {
						if(child is Document.Widget) {
							if((child as Document.Widget).tag == "item")
								gtk.append(create_widget(child as Document.Widget) as Gtk.MenuItem);
						}
					}
					rt = gtk;
				break;
				case "item":
					string label = widget.get("label");
					Gtk.MenuItem gtk;
					switch(label) {
						case "&":
						gtk = new Gtk.TearoffMenuItem();
						break;
						case "|":
						gtk = new Gtk.SeparatorMenuItem();
						break;
						default:
						gtk = new Gtk.MenuItem.with_label(widget.get("label"));
						break;
					}
					message("adding a menu item %s", label);
					foreach(weak XML.Node child in widget.children) {
						if(child is Document.Widget) {
							if((child as Document.Widget).tag == "menu")
								gtk.submenu = create_widget(child as Document.Widget);
						}
					}
					if(widget.get("visible") == "false" ) gtk.visible = false; else gtk.visible = true;
					if(widget.get("sensitive") == "false" ) gtk.sensitive = false; else gtk.sensitive = true;
					rt = gtk;
				break;
			}
			widget.set_data_full("gtk", rt.ref(), g_object_unref);
			return rt;
		}
		private void clean() {
			foreach(weak Gtk.Widget w in this.get_children()){
				this.remove(w);
			}
		}
		public void view(string menuname) {
			clean();	
			Document.Widget widget = document.lookup(menuname) as Document.Widget;
			foreach(weak XML.Node child in widget.children) {
				if(child is Document.Widget) {
					if((child as Document.Widget).tag == "item") {
						this.append(create_widget(child as Document.Widget) as Gtk.MenuItem);
					}
				}
			}
		}
	}

}

