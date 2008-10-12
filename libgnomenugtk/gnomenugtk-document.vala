using GLib;
using Gnomenu;
using GMarkupDoc;
using GtkAQD;

namespace GnomenuGtk {
	protected class Document : GMarkupDoc.Document, GMarkupDoc.DocumentModel {
		HashTable<weak string, weak Gtk.Widget> dict_nw;
		protected class Widget:GMarkupDoc.Tag {
			private Widget(Document document, string tag) {
				this.document = document;
				this.tag = document.S(tag);
			}
			public override void dispose() {
				base.dispose();
			}
		}
		public Document() {}
		construct {
			dict_nw = new HashTable<weak string, weak Gtk.Widget>(str_hash, str_equal);
			this.activated += (document, node, detail) => {
				weak Gtk.Widget widget = (document as Document).dict_nw.lookup(node.name);
				if(widget is Gtk.MenuItem) (widget as Gtk.MenuItem).activate();
				if(widget is GtkAQD.MenuBar) {
					bool local = (widget as GtkAQD.MenuBar).local;
					(widget as GtkAQD.MenuBar).local = !local;
				}
			};
		}
		public GMarkupDoc.Tag CreateTag(string tag) {
			return new Widget(this, tag);
		}
		private void list_to_array(List<weak string>? l, ref string[] array){
			array.resize((int)l.length());
			int i = 0;
			foreach(weak string s in l) {
				array[i] = s;
				i++;
			}
		}
		protected Document.Widget CreateWidget(string type, string name) {
			{
				weak GMarkupDoc.Node node = dict.lookup(name);
				if(node != null) return node as Document.Widget;
			}
			weak Gtk.Widget gtk = dict_nw.lookup(name);
			List<weak string> names;
			List<weak string> values;
			names.append("name");
			values.append(name);
			if(gtk is Gtk.MenuItem) { 
				GtkAQD.MenuItem item = gtk as GtkAQD.MenuItem;
				item.notify["visible"] += item_property_notify;
				item.notify["sensitive"] += item_property_notify;
				item.notify["no-show-all"] += item_property_notify;
				if(!item.visible) {
					names.append("visible");
					values.append("false");
				}
				if(item.no_show_all) {
					names.append("no-show-all");
					values.append("true");
				}
				if(!item.sensitive) {
					names.append("sensitive");
					values.append("false");
				}
				names.append("label");
				if(gtk is Gtk.TearoffMenuItem) {
					values.append("&");
				} else if(gtk is Gtk.SeparatorMenuItem) {
					values.append("|");
				} else  {
					weak Gtk.Label l = find_menu_item_label(gtk);
					if(l!= null) {
						l.notify["label"] += item_property_notify;
						gtk.set_data_full("old-label", l.ref(), g_object_unref);
						values.append(l.label);
					} else 
						values.append("unknown");
					item.label_set += item_label_set;
					if(gtk is Gtk.CheckMenuItem) {
						Gtk.CheckMenuItem c = gtk as Gtk.CheckMenuItem;
						gtk.notify["active"] += item_property_notify;
						gtk.notify["draw-as-radio"] += item_property_notify;
						gtk.notify["inconsistent"] += item_property_notify;
						names.append("active");
						values.append(c.active?"true":"false");
						names.append("draw-as-radio");
						values.append(c.draw_as_radio?"true":"false");
						names.append("inconsistent");
						values.append(c.inconsistent?"true":"false");
					}
					if(gtk is Gtk.ImageMenuItem) {
						Gtk.ImageMenuItem i = gtk as Gtk.ImageMenuItem;
						if((i.image is Gtk.Image)) {
							Gtk.Image image = i.image as Gtk.Image;
							set_native_name(image, get_native_name(gtk));
							image.notify["icon-name"] += item_property_notify;
							image.notify["stock"] += item_property_notify;
							switch(image.storage_type) {
								case Gtk.ImageType.ICON_NAME:
									names.append("icon-name");
									values.append(image.icon_name);
								break;
								case Gtk.ImageType.STOCK:
									names.append("icon-stock");
									values.append(image.stock);
								break;
							}
							i.set_data_full("old-image", image.ref(), g_object_unref);
						}
						i.notify["image"] += item_image_notify;
					}
				}
			}
			string[] anames = new string[1];
			string[] avalues = new string[1];
			list_to_array(names, ref anames);
			list_to_array(values, ref avalues);
			Widget node = CreateTagWithAttributes(type, anames, avalues) as Widget;
			return node;
		}
		public weak string wrap(Gtk.Widget widget) {
			weak string name = get_native_name(widget);
			if(name != null) return name;
			int id = Singleton.instance().unique;
			name = S("%s%d".printf(widget.get_type().name(), id));
			set_native_name(widget, name);
			dict_nw.insert(name, widget);
			return name;
		}
		public void unwrap(Gtk.Widget widget) {
			weak string name = get_native_name(widget);
			disconnect_signals(widget);
			if(name != null) {
				debug("GtkWidget %s is removed: %u", name, widget.ref_count);
				this.dict_nw.remove(name); // because ~WidgetNode is not always invoked?
				weak Document.Widget node = this.dict.lookup(name) as Document.Widget;
				if(node != null){
					if(node.parent == null) {
						assert(false);
					}
					node.parent.remove(node);
				}
			}
			set_native_name(widget, null);
		}
		private void disconnect_signals(GLib.Object object) {
			if(object is Gtk.MenuItem) {
				(object as Gtk.MenuItem).notify -= item_property_notify;
				(object as GtkAQD.MenuItem).label_set -= item_label_set;
				weak Gtk.Label label = find_menu_item_label(object as Gtk.Widget);
				if(label != null) {
					/*
					label.notify["label"] -= item_property_notify; VALA BUG
					*/
					label.notify -= item_property_notify;
				}
			}
			if(object is Gtk.ImageMenuItem) {
				(object as Gtk.ImageMenuItem).notify -= item_image_notify;
				weak Gtk.Image image = ((object as Gtk.ImageMenuItem).image as Gtk.Image);
				if(image != null) {
					/*
					image.notify["icon-name"] -= item_property_notify;
					image.notify["stock"] -= item_property_notify; VALA BUG
					*/
					image.notify -= item_property_notify; 
				}
			}
		}
		private static weak string get_native_name(GLib.Object? w) {
			return (string) ((GLibCompat.constpointer)w).get_data("native-name");
		}
		private static void set_native_name(GLib.Object? w, string? name) {
			((GLibCompat.constpointer)w).set_data("native-name", (void*) name);
		}
		private void item_property_notify(Gtk.Widget w, ParamSpec pspec) {
			debug("item_property_notify %s( %s).%s", get_native_name(w), w.get_type().name(), pspec.name);
			weak Document.Widget node = dict.lookup(get_native_name(w)) as Document.Widget;
			if(node == null) {
				warning("no xml node found for widget %s( %s)", get_native_name(w), w.get_type().name());
				return;
			}
			string val;
			if(pspec.value_type == typeof(string)) {
				w.get(pspec.name, out val, null);
			}
			if(pspec.value_type == typeof(bool)) {
				bool b;
				w.get(pspec.name, out b, null);
				val = b.to_string();
			}
			if(pspec.value_type == typeof(int)) {
				int i;
				w.get(pspec.name, out i, null);
				val = i.to_string();
			}
			node.set(pspec.name, val);
		}
		private void item_label_set(Gtk.Widget w, Gtk.Label? l) {
			weak Document.Widget node = dict.lookup(get_native_name(w)) as Document.Widget;
			if(node != null) {
				weak Gtk.Label old_label = (Gtk.Label) w.get_data("old-label");
				if(l != old_label) {
					if(old_label != null) {
						set_native_name(old_label, null);
						/*
						old_label.notify["label"] -= item_property_notify;
						*/
						old_label.notify -= item_property_notify;
					}
					if(l!= null) {
						l.notify["label"] += item_property_notify;
						node.set("label", l.label);
						w.set_data_full("old-label", l.ref(), g_object_unref);
					} else 
						w.set_data("old-label", null);
				}
			}
		}
		private void item_image_notify(Gtk.Widget w, ParamSpec pspec) {
			Gtk.ImageMenuItem item = w as Gtk.ImageMenuItem;
			weak Document.Widget node = dict.lookup(get_native_name(w)) as Document.Widget;
			if(w != null) {
				Gtk.Image image = item.image as Gtk.Image;
				weak Gtk.Image old_image = (Gtk.Image) item.get_data("old-image");
				if(image != old_image) {
					if(old_image != null) {
						set_native_name(old_image, null);
						/*
						old_image.notify["icon-name"] -= item_property_notify;
						old_image.notify["stock"] -= item_property_notify;
						*/
						old_image.notify -= item_property_notify; /*Vala bug detailed signal not removed*/
					}
					if(image != null) {
						set_native_name(image, get_native_name(w));
						image.notify["icon-name"] += item_property_notify;
						image.notify["stock"] += item_property_notify;
						switch(image.storage_type) {
							case Gtk.ImageType.ICON_NAME:
								w.set("icon-name", image.icon_name);
							break;
							case Gtk.ImageType.STOCK:
								w.set("icon-stock", image.stock);
							break;
						}
						item.set_data_full("old-image", image.ref(), g_object_unref);
					} else {
						item.set_data("old-image", null);
					}
				}
			}
		}
		private static weak Gtk.Label? find_menu_item_label(Gtk.Widget widget) {
			Queue<weak Gtk.Widget> q = new Queue<weak Gtk.Widget>();
			q.push_tail(widget);
			while(!q.is_empty()) {
				weak Gtk.Widget w = q.pop_head();
				if(w is Gtk.Container) {
					weak List<weak Gtk.Widget> children = (w as Gtk.Container).get_children();
					foreach(weak Gtk.Widget child in children){
						q.push_tail(child);
					}
				}
				if(w is Gtk.Label) {
					set_native_name(w, get_native_name(widget));
					return w as Gtk.Label;
				}
			}
			return null;
		}
	}
}
