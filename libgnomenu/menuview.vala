using GLib;
using Gtk;
using Gnomenu;
using GtkAQD;
using XML;

namespace Gnomenu {
	public class MenuView : GtkAQD.MenuBar {
		private XML.Document? _document;
		public weak XML.Document? document {
			get {
				return _document;
			} set {
				if(_document != null) {
					_document.inserted -= document_inserted;
					_document.updated -= document_updated;
					_document.removed -= document_removed;
				}
				_document = value;
				clean();	
				if(_document != null) {
					document.inserted += document_inserted;
					document.updated += document_updated;
					document.removed += document_removed;
					foreach(weak XML.Node child in document.root.children) {
						if(child is Document.Widget) {
							if((child as Document.Widget).tag == "item") {
								this.append(create_widget(child as Document.Widget) as Gtk.MenuItem);
							}
						}
					}
				}
			}
		}
		public MenuView(Document? document) {
			this.document = document;
			this.local = true;
		}
		private weak Gtk.Widget create_widget(Document.Widget node) {
			Gtk.Widget rt;
			weak Gtk.Widget gtk = (Gtk.Widget) node.get_data("gtk");
			message("creating node %s", node.name);
			if(gtk != null) return gtk;
			switch(node.tag) {
				case "menu":
					Gtk.MenuShell gtk = new Gtk.Menu();
					message("adding a menu");
					foreach(weak XML.Node child in node.children) {
						message("%s", child.get_type().name());
						if(child is Document.Widget) {
							switch((child as Document.Widget).tag){
								case "item":
								case "check":
								gtk.append(create_widget(child as Document.Widget) as Gtk.MenuItem);
								break;
							}
						}
					}
					rt = gtk;
				break;
				case "item":
				case "check":
					string label = node.get("label");
					Gtk.MenuItem gtk;
					switch(label) {
						case "&":
						gtk = new Gtk.TearoffMenuItem();
						gtk.activate += menu_item_activated;
						string[] p = {"visible", "sensitive"};
						update_properties(gtk, node, p);
						break;
						case "|":
						gtk = new Gtk.SeparatorMenuItem();
						gtk.activate += menu_item_activated;
						string[] p = {"visible", "sensitive"};
						update_properties(gtk, node, p);
						break;
						default:
						switch(node.tag) {
							case "check":
								gtk = new Gtk.CheckMenuItem.with_mnemonic(node.get("label"));
								gtk.activate += menu_item_activated;
								string[] p = {"visible", "sensitive", "label", "active", "inconsistent", "draw-as-radio"};
								update_properties(gtk, node, p);
							break;
							case "item":
								gtk = new Gtk.MenuItem.with_mnemonic(node.get("label"));
								gtk.activate += menu_item_activated;
								string[] p = {"visible", "sensitive", "label"};
								update_properties(gtk, node, p);
							break;
						}
						break;
					}
					message("adding a menu item %s", label);
					foreach(weak XML.Node child in node.children) {
						if(child is Document.Widget) {
							if((child as Document.Widget).tag == "menu")
								gtk.submenu = create_widget(child as Document.Widget);
						}
					}

					rt = gtk;
				break;
				default:
				message("skipping tag %s", node.tag);
				break;
			}
			rt.set_data("node", node);
			node.set_data_full("gtk", rt.ref(), g_object_unref);
			return rt;
		}
		private void clean() {
			foreach(weak Gtk.Widget w in this.get_children()){
				this.remove(w);
			}
		}
		private void document_inserted(XML.Document document, XML.Node p, XML.Node n, int pos) {
			if(!(n is Document.Widget)) return;
			weak Document.Widget node = n as Document.Widget;
			if(p == document.root ) {
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
					case "check":
						Gtk.MenuItem pgtk = (Gtk.MenuItem) p.get_data("gtk");
						pgtk.submenu = create_widget(node);
					break;
				}
			}
		}
		private void document_removed(XML.Document document, XML.Node p, XML.Node n) {
			if(!(n is Document.Widget)) return;
			weak Document.Widget node = n as Document.Widget;
			if(p == document.root) {
				message("%s", n.get_type().name());
				this.remove((Gtk.Widget)node.get_data("gtk"));
				return;
			}
			weak Document.Widget parent = p as Document.Widget;
			if(parent != null && node != null) {
				switch(parent.tag) {
					case "menu":
						Gtk.MenuShell pgtk = (Gtk.MenuShell) p.get_data("gtk");
						pgtk.remove((Gtk.Widget)node.get_data("gtk"));
					break;
					case "item":
					case "check":
						Gtk.MenuItem pgtk = (Gtk.MenuItem) p.get_data("gtk");
						pgtk.submenu = null;
					break;
				}
			}
		}
		private void menu_item_activated (Gtk.MenuItem o) {
			weak Document.Widget widget = (Document.Widget) o.get_data("node");
			if(widget != null);
				widget.activate();
		}
		private void update_properties(Gtk.Widget gtk, Document.Widget node, string[] props) {
			foreach(weak string s in props) {
				update_property(gtk, node, s);
			}
		}
		private void update_property(Gtk.Widget gtk, Document.Widget node, string prop) {
				if(gtk is Gtk.MenuItem) {
					(gtk as Gtk.MenuItem).activate -= menu_item_activated;
				}
				switch(prop) {
					case "label":
						Gtk.Label label = (gtk as Gtk.Bin).get_child() as Gtk.Label;
						label.label = node.get("label");
					break;
					case "visible":
					case "sensitive":
						if(node.get(prop) == "false")
							gtk.set(prop, false, null);
						else
							gtk.set(prop, true, null);
					break;
					case "active":
					case "inconsistent":
					case "draw-as-radio":
						if(node.get(prop) == "true")
							gtk.set(prop, true, null);
						else
							gtk.set(prop, false, null);
					break;
				}
				if(gtk is Gtk.MenuItem) {
					(gtk as Gtk.MenuItem).activate += menu_item_activated;
				}
		}
		private void document_updated(XML.Document document, XML.Node n, string prop) {
			if(!(n is Document.Widget)) return;
			weak Document.Widget node = n as Document.Widget;
			if(node != null) {
				switch(node.tag) {
					case "menu":
					break;
					case "item":
					case "check":
						Gtk.MenuItem gtk = (Gtk.MenuItem) node.get_data("gtk");
						update_property(gtk, node, prop);
					break;
				}
			}
			
		}
	}

}

