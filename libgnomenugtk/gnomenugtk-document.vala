using GLib;
using Gnomenu;
using GMarkupDoc;
using GtkAQD;

namespace GnomenuGtk {
	protected class Document : GMarkupDoc.Document, GMarkupDoc.DocumentModel {
		HashTable<weak string, weak Gtk.Widget> dict_nw;
		public class Widget:GMarkupDoc.Tag {
			private weak Gtk.Widget _gtk;
			public Gtk.Widget gtk {
				get {return _gtk;}
				set {
					if(gtk != null) 
						gtk.weak_unref(weak_ref_notify, this);
					_gtk = value;
					gtk.weak_ref(weak_ref_notify, this);
					connect();
				}
			}
			private Widget(Document document) {
				this.document = document;
			}
			private static void weak_ref_notify(void* data, GLib.Object object){
				Widget _this = (Widget) data;
				if(_this.parent != _this.document.orphan) {
					_this.parent.remove(_this);
				}
				_this.document.DestroyNode(_this);
				_this._gtk = null;
			}
			private void set_bool_def(string prop, bool value, bool def) {
				if(value != def)
					set_bool(prop, value);
				else
					set(prop, null);
			}
			private void set_bool(string prop, bool value) {
				set(prop, value?"true":"false");
			}
			private void connect_to_menu_item() {
				GtkAQD.MenuItem item = gtk as GtkAQD.MenuItem;
				item.notify["visible"] += item_property_notify;
				item.notify["sensitive"] += item_property_notify;
				item.notify["no-show-all"] += item_property_notify;
				set_bool_def("visible", item.visible, true);
				set_bool_def("no-show-all", item.no_show_all, false);
				set_bool_def("sensitive", item.sensitive, true);
				if(gtk is Gtk.TearoffMenuItem) set("label", "&");
				if(gtk is Gtk.SeparatorMenuItem) set("label", "|");
				weak Gtk.Label l = find_menu_item_label(gtk);
				if(l!= null) {
					l.notify["label"] += item_property_notify;
					gtk.set_data_full("old-label", l.ref(), g_object_unref);
					set("label", l.label);
				}
				item.label_set += item_label_set;
			}
			private void connect_to_check_menu_item() {
				Gtk.CheckMenuItem c = gtk as Gtk.CheckMenuItem;
				gtk.notify["active"] += item_property_notify;
				gtk.notify["draw-as-radio"] += item_property_notify;
				gtk.notify["inconsistent"] += item_property_notify;
				set_bool("active", c.active);
				set_bool_def("draw-as-radio", c.draw_as_radio, false);
				set_bool_def("inconsistent", c.inconsistent, false);

			}
			private void connect_to_image_menu_item() {
				Gtk.ImageMenuItem i = gtk as Gtk.ImageMenuItem;
				if((i.image is Gtk.Image)) {
					Gtk.Image image = i.image as Gtk.Image;
					set_native_name(image, name);
					image.notify["icon-name"] += item_property_notify;
					image.notify["stock"] += item_property_notify;
					switch(image.storage_type) {
						case Gtk.ImageType.ICON_NAME:
							set("icon-name", image.icon_name);
						break;
						case Gtk.ImageType.STOCK:
							set("icon-stock", image.stock);
						break;
					}
					i.set_data_full("old-image", image.ref(), g_object_unref);
				}
				i.notify["image"] += item_image_notify;
			}
			private void connect() {
				this.tag = translate_gtk_type(gtk);
				List<weak string> names;
				List<weak string> values;
				if(gtk is Gtk.MenuItem) { 
					connect_to_menu_item();
					if(gtk is Gtk.CheckMenuItem) connect_to_check_menu_item();
					if(gtk is Gtk.ImageMenuItem) connect_to_image_menu_item();
				}
			}
			private void item_property_notify(Gtk.Widget w, ParamSpec pspec) {
				debug("item_property_notify %s( %s).%s", this.name, w.get_type().name(), pspec.name);
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
				this.set(pspec.name, val);
			}
			private void item_label_set(Gtk.Widget w, Gtk.Label? l) {
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
						this.set("label", l.label);
						w.set_data_full("old-label", l.ref(), g_object_unref);
					} else 
						w.set_data("old-label", null);
				}
			}
			private void item_image_notify(Gtk.Widget gtk, ParamSpec pspec) {
				Gtk.ImageMenuItem item = gtk as Gtk.ImageMenuItem;
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
						set_native_name(image, this.name);
						image.notify["icon-name"] += item_property_notify;
						image.notify["stock"] += item_property_notify;
						switch(image.storage_type) {
							case Gtk.ImageType.ICON_NAME:
								this.set("icon-name", image.icon_name);
							break;
							case Gtk.ImageType.STOCK:
								this.set("icon-stock", image.stock);
							break;
						}
						item.set_data_full("old-image", image.ref(), g_object_unref);
					} else {
						item.set_data("old-image", null);
					}
				}
			}
		}
		public Document() {}
		construct {
			this.activated += (document, node, detail) => {
				weak Gtk.Widget widget = (node as Widget).gtk;
				if(widget is Gtk.MenuItem) (widget as Gtk.MenuItem).activate();
				if(widget is GtkAQD.MenuBar) {
					bool local = (widget as GtkAQD.MenuBar).local;
					(widget as GtkAQD.MenuBar).local = !local;
				}
			};
		}
		public GMarkupDoc.Tag CreateTag(string tag) {
			return new Widget(this);
		}
		public weak Document.Widget wrap(Gtk.Widget gtk) {
			weak string name = get_native_name(gtk);
			if(name != null) {
				weak Document.Widget rt = dict.lookup(name) as Document.Widget;
				if(rt != null) return rt;
			}
			int id = Singleton.instance().unique;
			name = S("%s%d".printf(gtk.get_type().name(), id));
			set_native_name(gtk, name);
			Widget node = CreateTag("widget") as Widget;
			node.set("name", name);
			node.gtk = gtk;
			return node;
		}
		private static weak string get_native_name(GLib.Object? w) {
			return (string) ((GLibCompat.constpointer)w).get_data("native-name");
		}
		private static void set_native_name(GLib.Object? w, string? name) {
			((GLibCompat.constpointer)w).set_data("native-name", (void*) name);
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
