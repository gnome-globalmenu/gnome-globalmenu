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
				_document.inserted -= document_inserted;
				_document.updated -= document_updated;
				_document.removed -= document_removed;
				_document = value;
				document.inserted += document_inserted;
				document.updated += document_updated;
				document.removed += document_removed;
				clean();	
				foreach(weak XML.Node child in document.root.children) {
					if(child is Document.Widget) {
						if((child as Document.Widget).tag == "item") {
							this.append(create_widget(child as Document.Widget) as Gtk.MenuItem);
						}
					}
				}
			}
		}
		public MenuView(Document? document) {
			this.document = document;
			this.local = true;
		}
		private weak Gtk.Widget create_widget(Document.Widget widget) {
			weak Gtk.Widget rt;
			weak Gtk.Widget gtk = (Gtk.Widget) widget.get_data("gtk");
			if(gtk != null) return gtk;
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
		private void document_inserted(XML.Document document, XML.Node p, XML.Node n, int pos) {
			weak Document.Widget node = n as Document.Widget;
			if(p is XML.Document.Root) {
				this.insert(create_widget(node) as Gtk.MenuItem, pos);
				return;
			}
			weak Document.Widget parent = p as Document.Widget;
			if(parent != null && node != null) {
				switch(parent.tag) {
					case "menu":
						Gtk.MenuShell pgtk = (Gtk.MenuShell) p.get_data("gtk");
						pgtk.insert(create_widget(node) as Gtk.MenuItem, pos);
					break;
					case "item":
						Gtk.MenuItem pgtk = (Gtk.MenuItem) p.get_data("gtk");
						pgtk.submenu = create_widget(node);
					break;
				}
			}
		}
		private void document_removed(XML.Document document, XML.Node p, XML.Node n) {
			if(p is XML.Document.Root) {
				this.remove((Gtk.Widget)p.get_data("gtk"));
				return;
			}
			weak Document.Widget parent = p as Document.Widget;
			weak Document.Widget node = n as Document.Widget;
			if(parent != null && node != null) {
				switch(parent.tag) {
					case "menu":
						Gtk.MenuShell pgtk = (Gtk.MenuShell) p.get_data("gtk");
						pgtk.remove((Gtk.Widget)p.get_data("gtk"));
					break;
					case "item":
						Gtk.MenuItem pgtk = (Gtk.MenuItem) p.get_data("gtk");
						pgtk.submenu = null;
					break;
				}
			}
		}
		private void document_updated(XML.Document document, XML.Node n, string prop) {
			weak Document.Widget node = n as Document.Widget;
			if(node != null) {
				switch(node.tag) {
					case "menu":
					break;
					case "item":
						Gtk.MenuItem gtk = (Gtk.MenuItem) node.get_data("gtk");
						switch(prop) {
							case "label":
								Gtk.Label label = gtk.get_child() as Gtk.Label;
								label.label = node.get("label");
							break;
							case "visible":
								if(node.get("visible") == "false" ) gtk.visible = false; else gtk.visible = true;
							break;
							case "sensible":
								if(node.get("sensitive") == "false" ) gtk.sensitive = false; else gtk.sensitive = true;
							break;
						}
					break;
				}
			}
			
		}
		public void view(string menuname) {
		}
	}

}

