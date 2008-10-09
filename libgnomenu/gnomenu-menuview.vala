using GLib;
using Gtk;
using GMarkupDoc;

namespace Gnomenu {
	public class MenuView : Gtk.MenuBar {
		private GMarkupDoc.Document? _document;
		public weak GMarkupDoc.Document? document {
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
					foreach(weak GMarkupDoc.Node child in document.root.children) {
						if(child is Document.NamedTag) {
							switch((child as Document.NamedTag).tag) {
								case "item":
								case "check":
								case "imageitem":
								this.append(create_widget(child as Document.NamedTag) as Gtk.MenuItem);
								break;
							}
						}
					}
				}
			}
		}
		public MenuView(Document? document) {
			this.document = document;
		}
		private Gdk.EventExpose __tmp__event;
		construct {
			this.set("local", true, null);
			this.expose_event += (widget, event)=> {
				if(0 != (widget.get_flags() & (Gtk.WidgetFlags.MAPPED | Gtk.WidgetFlags.VISIBLE))) {
					Gtk.paint_flat_box(widget.style,
							widget.window, (Gtk.StateType) widget.state,
							Gtk.ShadowType.NONE,
							event.area,
							widget, null, 0, 0, -1, -1);

					__tmp__event = event;
					(widget as GtkCompat.Container).forall (expose_child);
				}
				return true;
			};
			this.size_request += (widget, req) => {
				req.width = 0;
				req.height = 0;
			};
			this.size_allocate +=(widget, allocation) => {
				// do nothing;
				return;
			};
		}
		private void expose_child(Gtk.Widget widget) {
			this.propagate_expose(widget, __tmp__event);
		}
		private weak Gtk.Widget create_widget(Document.NamedTag node) {
			Gtk.Widget rt;
			weak Gtk.Widget gtk = (Gtk.Widget) node.get_data("gtk");
			//debug("creating node %s", node.name);
			if(gtk != null) return gtk;
			switch(node.tag) {
				case "menu":
					Gtk.MenuShell gtk = new Gtk.Menu();
					foreach(weak GMarkupDoc.Node child in node.children) {
						if(child is Document.NamedTag) {
							switch((child as Document.NamedTag).tag){
								case "item":
								case "imageitem":
								case "check":
								gtk.append(create_widget(child as Document.NamedTag) as Gtk.MenuItem);
								break;
							}
						}
					}
					rt = gtk;
				break;
				case "item":
				case "check":
				case "imageitem":
					string label = node.get("label");
					Gtk.MenuItem gtk;
					switch(label) {
						case "&":
						gtk = new Gtk.TearoffMenuItem();
						gtk.activate += menu_item_activated;
						string[] p = {"visible", "sensitive", "no-show-all"};
						update_properties(gtk, node, p);
						break;
						case "|":
						gtk = new Gtk.SeparatorMenuItem();
						gtk.activate += menu_item_activated;
						string[] p = {"visible", "sensitive", "no-show-all"};
						update_properties(gtk, node, p);
						break;
						default:
						switch(node.tag) {
							case "check":
								gtk = new Gtk.CheckMenuItem.with_mnemonic(node.get("label"));
								gtk.activate += menu_item_activated;
								string[] p = {"visible", "sensitive", "no-show-all", "label", "active", "inconsistent", "draw-as-radio"};
								update_properties(gtk, node, p);
							break;
							case "item":
								gtk = new Gtk.MenuItem.with_mnemonic(node.get("label"));
								gtk.activate += menu_item_activated;
								string[] p = {"visible", "sensitive", "no-show-all", "label"};
								update_properties(gtk, node, p);
							break;
							case "imageitem":
								gtk = new Gtk.ImageMenuItem.with_mnemonic(node.get("label"));
								gtk.activate += menu_item_activated;
								string[] p = {"visible", "sensitive", "no-show-all", "label", "icon-name","icon-stock"};
								update_properties(gtk, node, p);
							break;
						}
						break;
					}
					foreach(weak GMarkupDoc.Node child in node.children) {
						if(child is Document.NamedTag) {
							if((child as Document.NamedTag).tag == "menu")
								gtk.submenu = create_widget(child as Document.NamedTag);
						}
					}

					rt = gtk;
				break;
				default:
				debug("skipping tag %s", node.tag);
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
		private void document_inserted(GMarkupDoc.Document document, GMarkupDoc.Node p, GMarkupDoc.Node n, int pos) {
			if(!(n is Document.NamedTag)) return;
			weak Document.NamedTag node = n as Document.NamedTag;
			if(p == document.root ) {
				this.insert(create_widget(node) as Gtk.MenuItem, pos);
				return;
			}
			weak Document.NamedTag parent = p as Document.NamedTag;
			if(parent != null && node != null) {
				switch(parent.tag) {
					case "menu":
						Gtk.MenuShell pgtk = (Gtk.MenuShell) p.get_data("gtk");
						pgtk.insert(create_widget(node) as Gtk.MenuItem, pos);
					break;
					case "item":
					case "check":
					case "imageitem":
						Gtk.MenuItem pgtk = (Gtk.MenuItem) p.get_data("gtk");
						pgtk.submenu = create_widget(node);
					break;
				}
			}
		}
		private void document_removed(GMarkupDoc.Document document, GMarkupDoc.Node p, GMarkupDoc.Node n) {
			if(!(n is Document.NamedTag)) return;
			weak Document.NamedTag node = n as Document.NamedTag;
			if(p == document.root) {
				this.remove((Gtk.Widget)node.get_data("gtk"));
				return;
			}
			weak Document.NamedTag parent = p as Document.NamedTag;
			if(parent != null && node != null) {
				switch(parent.tag) {
					case "menubar":
					case "menu":
						weak Gtk.MenuShell pgtk = (Gtk.MenuShell) p.get_data("gtk");
						weak Gtk.MenuItem gtk = (Gtk.MenuItem)node.get_data("gtk");
						if(gtk != null && gtk.submenu != null) {
							gtk.submenu.popdown();
							gtk.submenu = null;
						}
						pgtk.remove((Gtk.Widget)node.get_data("gtk"));
					break;
					case "item":
					case "check":
					case "imageitem":
						Gtk.MenuItem pgtk = (Gtk.MenuItem) p.get_data("gtk");
						pgtk.submenu = null;
					break;
				}
			}
		}
		private void menu_item_activated (Gtk.MenuItem o) {
			weak Document.NamedTag widget = (Document.NamedTag) o.get_data("node");
			if(widget != null);
				widget.activate();
		}
		private void update_properties(Gtk.Widget gtk, Document.NamedTag node, string[] props) {
			foreach(weak string s in props) {
				update_property(gtk, node, s);
			}
		}
		private void update_property(Gtk.Widget gtk, Document.NamedTag node, string prop) {
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
					case "no-show-all":
					case "active":
					case "inconsistent":
					case "draw-as-radio":
						if(node.get(prop) == "true")
							gtk.set(prop, true, null);
						else
							gtk.set(prop, false, null);
					break;
					case "icon-name":
						if(node.get(prop) != null) {
							Gtk.Image image = new Gtk.Image.from_icon_name(
										node.get(prop), Gtk.IconSize.MENU);
							(gtk as Gtk.ImageMenuItem).image = image;
						}
					break;
					case "icon-stock":
						if(node.get(prop) != null) {
							Gtk.Image image = new Gtk.Image.from_stock(
										node.get(prop), Gtk.IconSize.MENU);
							(gtk as Gtk.ImageMenuItem).image = image;
						}
					break;
				}
				if(gtk is Gtk.MenuItem) {
					(gtk as Gtk.MenuItem).activate += menu_item_activated;
				}
		}
		private void document_updated(GMarkupDoc.Document document, GMarkupDoc.Node n, string prop) {
			if(!(n is Document.NamedTag)) return;
			weak Document.NamedTag node = n as Document.NamedTag;
			if(node != null) {
				switch(node.tag) {
					case "menu":
					break;
					case "item":
					case "check":
					case "imageitem":
						Gtk.MenuItem gtk = (Gtk.MenuItem) node.get_data("gtk");
						update_property(gtk, node, prop);
					break;
				}
			}
			
		}
	}

}

