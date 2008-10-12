using GLib;
using Gtk;
using GMarkupDoc;

namespace Gnomenu {
	public class MenuView : GtkCompat.Container {
		private DocumentModel? _document;
		private Gtk.MenuBar menubar;
		private Gtk.Widget arrow_button;
		public weak DocumentModel? document {
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
					document.root.set_data("gtk", this.menubar);
					foreach(weak GMarkupDoc.Node child in document.root.children) {
						if(child is GMarkupDoc.Tag) {
							switch((child as GMarkupDoc.Tag).tag) {
								case "item":
								case "check":
								case "imageitem":
								this.menubar.append(create_widget(child as GMarkupDoc.Tag) as Gtk.MenuItem);
								break;
							}
						}
					}
				}
			}
		}
		public MenuView(DocumentModel? document) {
			this.document = document;
		}
		~MenuView() {
			debug("menuview is finalized");
		}
		private Gdk.EventExpose __tmp__event;
		construct {
			this.set_flags(Gtk.WidgetFlags.NO_WINDOW);
			this.menubar = new Gtk.MenuBar();
			this.menubar.visible = true;
			this.menubar.set("local", true, null);
			this.menubar.set_parent(this);
			this.menubar.expose_event += (widget, event)=> {
				if(0 != (widget.get_flags() & (Gtk.WidgetFlags.MAPPED | Gtk.WidgetFlags.VISIBLE))) {
					Gtk.paint_flat_box(widget.style,
							widget.window, (Gtk.StateType) widget.state,
							Gtk.ShadowType.NONE,
							event.area,
							widget, null, 0, 0, -1, -1);

					__tmp__event = event;
					(widget as GtkCompat.Container).forall_children (expose_child);
				}
				return true;
			};
			this.arrow_button = new Gtk.ToggleButton.with_label(">");
			this.arrow_button.set_parent(this);
			this.size_request += (widget, req) => {
				this.menubar.size_request(req);
				this.arrow_button.size_request(req);
			};
			this.size_allocate +=(widget, allocation) => {
				Gdk.Rectangle arrow_alloc = allocation;
				Gdk.Rectangle menubar_alloc = allocation;
				Gtk.Requisition arrow_req;
				Gtk.Requisition menubar_req;

				this.menubar.get_child_requisition(out menubar_req);
				this.arrow_button.get_child_requisition(out arrow_req);
				
				if(menubar_req.width > allocation.width) {
					arrow_alloc.width = arrow_req.width;
					menubar_alloc.width -= arrow_req.width;
					arrow_alloc.x = menubar_alloc.width + allocation.x;
					this.arrow_button.size_allocate(arrow_alloc);
					this.arrow_button.visible = true;
				} else {
					this.arrow_button.visible = false;
				}
				this.menubar.size_allocate(menubar_alloc);
				return;
			};
		}
	    private void dump_alloc(Gdk.Rectangle allocation) {
				debug("width %d, height %d, x %d, y %d", allocation.width, allocation.height, allocation.x, allocation.y);
		}	
		private override void forall (bool include_internals, GtkCompat.Callback callback, void* data) {
			if(include_internals) {
				callback(this.arrow_button, data);
				callback(this.menubar, data);
			}
		}

		private void expose_child(Gtk.Widget widget) {
			this.menubar.propagate_expose(widget, __tmp__event);
		}
		private weak Gtk.Widget create_widget(GMarkupDoc.Tag node) {
			weak Gtk.Widget _gtk = (Gtk.Widget) node.get_data("gtk");
			//debug("creating node %s", node.name);
			if(_gtk != null) return _gtk;
			switch(node.tag) {
				case "menu":
					Gtk.MenuShell gtk = new Gtk.Menu();
					foreach(weak GMarkupDoc.Node child in node.children) {
						if(child is GMarkupDoc.Tag) {
							switch((child as GMarkupDoc.Tag).tag){
								case "item":
								case "imageitem":
								case "check":
								gtk.append(create_widget(child as GMarkupDoc.Tag) as Gtk.MenuItem);
								break;
							}
						}
					}
					gtk.set_data("node", node);
					node.set_data_full("gtk", gtk.ref(), g_object_unref);
				break;
				case "item":
				case "check":
				case "imageitem":
					string label = node.get("label");
					if(label == null) label = "";
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
								gtk = new Gtk.CheckMenuItem.with_mnemonic(label);
								gtk.activate += menu_item_activated;
								string[] p = {"visible", "sensitive", "no-show-all", "label", "active", "inconsistent", "draw-as-radio"};
								update_properties(gtk, node, p);
							break;
							case "item":
								gtk = new Gtk.MenuItem.with_mnemonic(label);
								gtk.activate += menu_item_activated;
								string[] p = {"visible", "sensitive", "no-show-all", "label"};
								update_properties(gtk, node, p);
							break;
							case "imageitem":
								gtk = new Gtk.ImageMenuItem.with_mnemonic(label);
								gtk.activate += menu_item_activated;
								string[] p = {"visible", "sensitive", "no-show-all", "label", "icon-name","icon-stock"};
								update_properties(gtk, node, p);
							break;
						}
						break;
					}
					foreach(weak GMarkupDoc.Node child in node.children) {
						if(child is GMarkupDoc.Tag) {
							if((child as GMarkupDoc.Tag).tag == "menu")
								gtk.submenu = create_widget(child as GMarkupDoc.Tag);
						}
					}
					gtk.set_data("node", node);
					node.set_data_full("gtk", gtk.ref(), g_object_unref);
				break;
				default:
				debug("skipping tag %s", node.tag);
				break;
			}
			weak Gtk.Widget rt = (Gtk.Widget) (node.get_data("gtk"));
			rt.destroy += (widget) => {
				GMarkupDoc.Tag node = (GMarkupDoc.Tag) widget.get_data("node");
				node.set_data("gtk", null);
			};
			return rt;
		}
		private void clean() {
			weak List<weak Gtk.Widget> l = this.menubar.get_children();
			foreach(weak Gtk.Widget w in l){
				w.destroy();
			}
		}
		private void document_inserted(DocumentModel document, GMarkupDoc.Node p, GMarkupDoc.Node n, int pos) {
			debug("inserted");
			if(!(n is GMarkupDoc.Tag)) return;
			weak GMarkupDoc.Tag node = n as GMarkupDoc.Tag;
			if(p == document.root ) {
				this.menubar.insert(create_widget(node) as Gtk.MenuItem, pos);
				return;
			}
			weak GMarkupDoc.Tag parent = p as GMarkupDoc.Tag;
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
		private void document_removed(DocumentModel document, GMarkupDoc.Node parent, GMarkupDoc.Node node) {
			if(!(node is GMarkupDoc.Tag)) return;
			debug("removed %s from %s", node.name, parent.name);
			if(parent != null && node != null) {
				weak Gtk.Widget pgtk = (Gtk.Widget) parent.get_data("gtk");
				weak Gtk.Widget gtk = (Gtk.Widget)node.get_data("gtk");
				if((pgtk is Gtk.MenuShell) && (gtk is Gtk.MenuItem)) {
					debug("removing from menushell");
					(pgtk as Gtk.Container).remove(gtk);
				}
				if(pgtk is Gtk.MenuItem && (gtk is Gtk.MenuShell)) {
					(pgtk as Gtk.MenuItem).submenu = null;
				}
				debug("gtk ref_count = %u", gtk.ref_count);
				gtk.destroy();
			}
		}
		private void menu_item_activated (Gtk.MenuItem o) {
			weak GMarkupDoc.Tag widget = (GMarkupDoc.Tag) o.get_data("node");
			if(widget != null) {
				debug("activated");
				document.activate(widget, 0);
			}
		}
		private void update_properties(Gtk.Widget gtk, GMarkupDoc.Tag node, string[] props) {
			foreach(weak string s in props) {
				update_property(gtk, node, s);
			}
		}
		private void update_property(Gtk.Widget gtk, GMarkupDoc.Tag node, string prop) {
				if(gtk is Gtk.MenuItem) {
					(gtk as Gtk.MenuItem).activate -= menu_item_activated;
				}
				switch(prop) {
					case "label":
						Gtk.Label label = (gtk as Gtk.Bin).get_child() as Gtk.Label;
						string label_text = node.get("label");
						if(label_text == null) label_text = "";
						label.label = label_text;
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
		private void document_updated(DocumentModel document, GMarkupDoc.Node n, string prop) {
			if(!(n is GMarkupDoc.Tag)) return;
			weak GMarkupDoc.Tag node = n as GMarkupDoc.Tag;
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

