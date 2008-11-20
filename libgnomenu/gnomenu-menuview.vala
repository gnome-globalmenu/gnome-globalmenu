using GLib;
using Gtk;
using GMarkup;

namespace Gnomenu {
	private class WidgetFactory: GLib.Object {
		private HashTable<weak GMarkup.Node, weak Gtk.Widget> node2widget;
		private HashTable<weak Gtk.Widget, weak GMarkup.Node> widget2node;
		public enum Flags {
			MENUBAR_AS_MENUBAR,
			MENUBAR_AS_MENU
		}
		public Flags flags {get; construct;}
		WidgetFactory(Flags flags) {
			this.flags = flags;
		}
		construct {
			node2widget = new HashTable<weak GMarkup.Node, weak Gtk.Widget>(direct_hash, direct_equal);
			widget2node = new HashTable<weak Gtk.Widget, weak GMarkup.Node>(direct_hash, direct_equal);
		}
		public void clear() {
			List<weak Gtk.Widget> list = node2widget.get_values();
			foreach(weak Gtk.Widget widget in list) {
				widget.destroy();
				widget.unref();
			}
			List<weak GMarkup.Node> list2 = node2widget.get_keys();
			foreach(weak GMarkup.Node node in list2) {
				node.weak_unref(weak_ref_notify, this);
			}
			node2widget.remove_all();
			widget2node.remove_all();
		}
		public override void dispose() {
			clear();
		}
		public weak Gtk.Widget? getWidget(GMarkup.Node node) {
			return node2widget.lookup(node);
		}
		public weak GMarkup.Node? getNode(Gtk.Widget widget) {
			return widget2node.lookup(widget);
		}
		private Gtk.MenuShell createMenu(GMarkup.Node node, bool recur) {
			Gtk.Menu menu = new Menu();
			if(recur) {
				foreach(weak GMarkup.Node child in node.children) {
					weak Gtk.Widget widget = createWidget(child, true);
					if(widget != null) {
						menu.append(widget as Gtk.MenuItem);
					}
				}
			}
			return menu;	
		}
		private Gtk.MenuShell createMenuBar(GMarkup.Node node, bool recur) {
			Gtk.MenuBar menubar = new MenuBar();
			menubar.visible = true;
			menubar.set("local", true, null);
			menubar.expose_event += (widget, event)=> {
				if(0 != (widget.get_flags() & (Gtk.WidgetFlags.MAPPED | Gtk.WidgetFlags.VISIBLE))) {
					Gtk.paint_flat_box(widget.style,
							widget.window, (Gtk.StateType) widget.state,
							Gtk.ShadowType.NONE,
							event.area,
							widget, null, 0, 0, -1, -1);
					weak List<weak Gtk.Widget> children = (widget as Gtk.Container).get_children();
					foreach(weak Gtk.Widget child in children) {
						widget.propagate_expose(child, event);
					}
				}
				return true;
			};
			if(recur) {
				foreach(weak GMarkup.Node child in node.children) {
					weak Gtk.Widget widget = createWidget(child, true);
					if(widget != null) {
						menubar.append(widget as Gtk.MenuItem);
					}
				}
			}
			return menubar;
		}
		private weak Gtk.MenuItem createMenuItem(GMarkup.Node node, bool recur) {
			Gtk.MenuItem item = new Gtk.MenuItem();
			if(recur) {
				foreach(weak GMarkup.Node child in node.children) {
					if(child.name == "menu") 
						item.submenu = createWidget(child, true);
				}
			}
			item.activate += menu_item_activated;
			return item;
		}
		public weak Gtk.Widget? createWidget(GMarkup.Node node, bool recur = false) {
			if(getWidget(node) != null) {
				return getWidget(node);
			}
			Gtk.Widget widget;
			weak string name = node.name;
			if(flags == Flags.MENUBAR_AS_MENU) {
				name = "menu";
			}
			switch(name) {
				case "menubar":
					widget = createMenuBar(node, recur);
				break;
				case "menu":
					widget = createMenu(node, recur);
				break;
				case "item":
				case "check":
				case "imageitem":
				case "tearoff":
					widget = createMenuItem(node, recur);
				break;
				default:
					widget = null;
				break;
			}
			if(widget != null) {
				node2widget.insert(node, widget.ref() as Gtk.Widget);
				widget2node.insert(widget, node);
				node.weak_ref(weak_ref_notify, this);
				syncWidget(node, null);
				return node2widget.lookup(node);
			} else {
				return null;
			}
		}
		private void sync_bool_def(GMarkup.Node node, string prop, string widget_prop, bool def) {
			weak Gtk.Widget widget = getWidget(node);
			if(def == false) {
				if(node.get(prop) == "true")
					widget.set(widget_prop, true, null);
				else
					widget.set(widget_prop, false, null);
			} else {
				if(node.get(prop) == "false")
					widget.set(widget_prop, false, null);
				else
					widget.set(widget_prop, true, null);
			}
		}
		private void syncMenuItem(GMarkup.Node node, string? prop) {
			weak Gtk.MenuItem item = getWidget(node) as Gtk.MenuItem;
			assert(item != null);
			item.activate -= menu_item_activated;
			if( prop == null || prop == "label" ||
				prop == null || prop == "accel") {
				string label_text = node.get("label");
				Gtk.Widget label = item.get_child();
				if(label_text == "|") {
					item.remove(label);
					Gtk.HSeparator sep = new Gtk.HSeparator();
					sep.visible = true;
					item.add(sep);
				}
				string accel_text = node.get("accel");
				StringBuilder builder = new StringBuilder("");
				if(label_text != null) {
					for(int i=0; label_text[i]!=0; i++) {
						if(label_text[i]!='_') {
							builder.append_unichar(label_text[i]);
						}
					}
				}
				if(accel_text != null) {
					builder.append(" - "); 
					builder.append(accel_text);
				}
				if(!(label is Gtk.Label)) {
					(item as Gtk.Bin).remove(label);
					label = new Gtk.Label(builder.str);
					(item as Gtk.Bin).add(label);
				} else 
					(label as Gtk.Label).label = builder.str;
			}
			if( prop == null || prop == "active")
				sync_bool_def(node, "active", "active", false);
			if(	prop == null || prop == "inconsistent")
				sync_bool_def(node, "inconsistent", "inconsistent", false);
			if( prop == null || prop == "draw-as-radio")
				sync_bool_def(node, "draw-as-radio", "draw-as-radio", false);

			if( prop == null || prop == "icon-name") {
				if(node.get("icon-name") != null) {
					Gtk.Image image = new Gtk.Image.from_icon_name(
								node.get("icon-name"), Gtk.IconSize.MENU);
					(item as Gtk.ImageMenuItem).image = image;
				}
			}
			if( prop == null || prop == "icon-stock") {
				if(node.get("icon-stock") != null) {
					Gtk.Image image = new Gtk.Image.from_stock(
								node.get("icon-stock"), Gtk.IconSize.MENU);
					(item as Gtk.ImageMenuItem).image = image;
				}
			}
			item.activate += menu_item_activated;
		}
		private void syncMenuShell(GMarkup.Node node, string? prop) {
		
		}
		public void syncWidget(GMarkup.Node node, string? prop) {
			weak Gtk.Widget widget = getWidget(node);
			assert(widget != null);
				/*TODO: if prop == NULL refresh everything.*/
			if( prop == null || prop == "sensitive")
				sync_bool_def(node, "sensitive", "sensitive", true);
			if( prop == null || prop == "visible")
				sync_bool_def(node, "visible", "visible", true);
			if( prop == null || prop == "no-show-all")
				sync_bool_def(node, "no-show-all", "no-show-all", false);

			switch(node.name) {
				case "item":
				case "check":
				case "imageitem":
				case "tearoff":
					syncMenuItem(node, prop);
				break;
				case "menu":
				case "menubar":
					syncMenuShell(node, prop);
				break;
			}
		}
		private static void weak_ref_notify(void * _this, GLib.Object node_was) {
			weak WidgetFactory __this = (WidgetFactory) _this;
			weak Gtk.Widget widget = __this.node2widget.lookup((GMarkup.Node)node_was);
			__this.node2widget.remove((GMarkup.Node)node_was);
			__this.widget2node.remove(widget);
			widget.destroy();
			widget.unref();
		}
		private void menu_item_activated (Gtk.MenuItem item) {
			weak GMarkup.Node node = getNode(item);
			if(node != null) {
				this.activated(node);
			}
		}
		public signal void activated (GMarkup.Node node);
	}
	public class MenuView : GtkCompat.Container {
		private DocumentModel? _document;
		private weak Gtk.MenuBar menubar;
		private weak Gtk.Menu menu;
		private Gtk.Widget arrow_button;
		public signal void activated(GMarkup.Node node);
		WidgetFactory[] factories;
		private const int MENUBAR = 0;
		private const int MENU = 1;
		public weak DocumentModel? document {
			get {
				return _document;
			}
			set {
				if(_document != null) {
					document.inserted -= document_inserted;
					document.updated -= document_updated;
					document.removed -= document_removed;
				}
				_document = value;
				foreach(WidgetFactory factory in factories) {
					factory.clear();
				}
				if(_document != null) {
					document.inserted += document_inserted;
					document.updated += document_updated;
					document.removed += document_removed;
					menubar = factories[MENUBAR].createWidget(document.root) as Gtk.MenuBar;
					menubar.set_parent(this);
					menubar.set_style(this.style);
					menu = factories[MENU].createWidget(document.root) as Gtk.Menu;
					menu.deactivate += menu_deactivated;
				}
				this.queue_resize();
			}
		}
		private void menu_deactivated (Gtk.Menu menu) {
			(arrow_button as Gtk.ToggleButton).active = false;
		}
		private void document_destroyed(DocumentModel document) {
			this.document = null;
			debug("view releases the document");
		}
		public MenuView() { }
		private Gdk.EventExpose __tmp__event;
		construct {
			this.factories = new WidgetFactory[2];
			this.factories[MENUBAR] = new WidgetFactory(WidgetFactory.Flags.MENUBAR_AS_MENUBAR);
			this.factories[MENU] = new WidgetFactory(WidgetFactory.Flags.MENUBAR_AS_MENU);
			foreach(WidgetFactory factory in factories) {
				factory.activated += (factory, node) => {
					activated(node);
				};
			}
			this.set_flags(Gtk.WidgetFlags.NO_WINDOW);
			(this as GtkCompat.Widget).style_set += (widget, style)=> {
				this.arrow_button.set_style(this.style);
				if(this.menubar != null)
					this.menubar.set_style(this.style);
			};
			this.arrow_button = new Gtk.ToggleButton.with_label(">");
			this.arrow_button.set_parent(this);
			(this.arrow_button as Gtk.ToggleButton).toggled += (button) => {
				if((button as Gtk.ToggleButton).active) {
					if(this.menu == null || this.menubar == null) return;
					weak List<weak Gtk.Widget> list = this.menu.get_children();
					foreach(weak Gtk.Widget child in list) {
						child.visible = false;
					}
					list = this.menubar.get_children();
					Gtk.Allocation menubar_alloc = this.menubar.allocation;
					foreach(weak Gtk.Widget child in list) {
						Gtk.Allocation child_alloc = child.allocation;
						if((child_alloc.x + child_alloc.width) > menubar_alloc.width) {
							weak GMarkup.Node node = factories[MENUBAR].getNode(child);
							weak Gtk.Widget menu_child = factories[MENU].getWidget(node); 
							menu_child.visible = child.visible;
						}
					}
					this.menu.popup(null, null, null, 0, Gtk.get_current_event_time());
				}
			};
			this.size_request += (widget, req) => {
				/* create the cached menu item sizes in menubar*/
				if(this.menubar != null)
					this.menubar.size_request(req);
				this.arrow_button.size_request(req);
			};
			this.size_allocate +=(widget, allocation) => {
				Gdk.Rectangle arrow_alloc = allocation;
				Gdk.Rectangle menubar_alloc = allocation;
				Gtk.Requisition arrow_req;
				Gtk.Requisition menubar_req;
				if(this.menubar != null) {
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
				} else {
					this.arrow_button.size_allocate(arrow_alloc);
					this.arrow_button.visible = true;
				}
				return;
			};
		}
	    private void dump_alloc(Gdk.Rectangle allocation) {
				debug("width %d, height %d, x %d, y %d", allocation.width, allocation.height, allocation.x, allocation.y);
		}	
		private override void forall (bool include_internals, GtkCompat.Callback callback, void* data) {
			if(include_internals) {
				callback(this.arrow_button, data);
				if(this.menubar != null)
					callback(this.menubar, data);
			}
		}

		private void document_inserted(DocumentModel document, GMarkup.Node parent, GMarkup.Node node, GMarkup.Node? ref_node) {
			foreach(WidgetFactory factory in factories) {
				weak Gtk.Widget widget = factory.createWidget(node, true);
				weak Gtk.Widget container = factory.getWidget(parent);
				int pos = parent.getPos(ref_node);
				if(container is Gtk.MenuShell) {
					(container as Gtk.MenuShell).insert(widget, pos);
				}
				if(container is Gtk.MenuItem) {
					(container as Gtk.MenuItem).submenu = widget;
				}
			}
		}
		private void document_removed(DocumentModel document, GMarkup.Node parent, GMarkup.Node node) {
			debug("removed %s from %s", node.name, parent.name);
			foreach(WidgetFactory factory in factories) {
				weak Gtk.Widget container = factory.getWidget(parent);
				weak Gtk.Widget widget = factory.getWidget(node);
				if((container is Gtk.MenuShell) && (widget is Gtk.MenuItem)) {
					(container as Gtk.Container).remove(widget);
				}
				if((container is Gtk.MenuItem) && (widget is Gtk.MenuShell)) {
					(container as Gtk.MenuItem).submenu = null;
				}
				debug("gtk ref_count = %u", widget.ref_count);
				/*Then we wait for the weak notify from the removed node to destroy the widget in the factory*/
			}
		}
		private void document_updated(DocumentModel document, GMarkup.Node node, string? prop) {
			foreach(WidgetFactory factory in factories) {
				factory.syncWidget(node, prop);
			}
		}
	}

}

