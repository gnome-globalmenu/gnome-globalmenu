using GLib;
using Gnomenu;
using GMarkup;
using GtkAQD;

namespace GnomenuGtk {
	protected class Document : GMarkup.Document, GMarkup.DocumentModel {
		HashTable<weak string, weak Gtk.Widget> dict_nw;
		public class Widget:GMarkup.Tag {
			private weak Gtk.Widget _gtk;
			public Gtk.Widget gtk {
				get {return _gtk;}
				set {
					if(gtk != null) 
						gtk.weak_unref((WeakNotify)weak_ref_notify, this);
					_gtk = value;
					gtk.weak_ref((WeakNotify)weak_ref_notify, this);
					connect();
				}
			}
			private Widget(Document document) {
				this.document = document;
			}
			private static void weak_ref_notify(void* data, void* object){
				Widget _this = (Widget) data;
				if(_this.parent != _this.document.orphan) {
					_this.parent.remove(_this);
				}
				set_native_name(object, null);
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
				if(gtk is Gtk.SeparatorMenuItem) {
					set("label", "|");
				} else {
					weak Gtk.Widget l = find_menu_item_label(gtk);
					if(l != null) {
						if(l is Gtk.Label) {
							l.notify["label"] += item_property_notify;
							set("label", (l as Gtk.Label).label);
							if(l is Gtk.AccelLabel) {
								l.notify["accel-widget"] += accel_label_notify;
								l.notify["accel-closure"] += accel_label_notify;
								accel_label_notify(l as Gtk.AccelLabel);
							}
						}
						if(l is Gtk.Separator|| l is Gtk.SeparatorMenuItem) {
							set("label", "|");
						}
						gtk.set_data_full("old-label", l.ref(), g_object_unref);
					} else {
						set("label", "|");
					}
					item.label_set += item_label_set;
				}
			}
			private void connect_to_check_menu_item() {
				Gtk.CheckMenuItem c = gtk as Gtk.CheckMenuItem;
				gtk.notify["active"] += item_property_notify;
				gtk.notify["draw-as-radio"] += item_property_notify;
				gtk.notify["inconsistent"] += item_property_notify;
				set_bool("active", c.active);
				set_bool_def("draw-as-radio", c.draw_as_radio, false);
				set_bool_def("inconsistent", c.inconsistent, false);
				if(gtk is Gtk.RadioMenuItem) {
					c.toggled += (gtk) => {
						weak SList<weak Gtk.Widget> group = (gtk as Gtk.RadioMenuItem).get_group();
						foreach (weak Gtk.Widget ww in group) {
							(ww as GLibCompat.Object).notify_property("active");
						}
					};
				}

			}
			private void connect_to_image(Gtk.Image image) {
				set_native_name(image, this.name);
				image.notify["icon-name"] += item_property_notify;
				image.notify["stock"] += item_property_notify;
				image.notify["file"] += item_property_notify;
				switch(image.storage_type) {
					case Gtk.ImageType.ICON_NAME:
						this.set("icon-name", image.icon_name);
					break;
					case Gtk.ImageType.STOCK:
						this.set("icon-stock", image.stock);
					break;
					default:
						if(image.file != null) 
							this.set("icon-file", image.file);
					break;
				}
			}
			private void connect_to_image_menu_item() {
				Gtk.ImageMenuItem i = gtk as Gtk.ImageMenuItem;
				if((i.image is Gtk.Image)) {
					Gtk.Image image = i.image as Gtk.Image;
					connect_to_image(image);
					i.set_data_full("old-image", image.ref(), g_object_unref);
				}
				i.notify["image"] += item_image_notify;
			}
			private void connect() {
				this.tag = translate_gtk_type(gtk);
				this.freeze();
				if(gtk is Gtk.MenuItem) { 
					connect_to_menu_item();
					if(gtk is Gtk.CheckMenuItem) connect_to_check_menu_item();
					if(gtk is Gtk.ImageMenuItem) connect_to_image_menu_item();
				}
				this.unfreeze();
				this.document.updated(this, null);
			}
			private void item_property_notify(Gtk.Widget w, ParamSpec pspec) {
				debug("item_property_notify %s( %s).%s", this.name, w.get_type().name(), pspec.name);
				Type type = pspec.value_type;
				weak string name = pspec.name;
				string val;
				if(type == typeof(string)) {
					w.get(name, out val, null);
				}
				if(type == typeof(bool)) {
					bool b;
					w.get(name, out b, null);
					val = b.to_string();
				}
				if(type == typeof(int)) {
					int i;
					w.get(name, out i, null);
					val = i.to_string();
				}
				if(w is Gtk.Image) {
					switch(pspec.name) {
						case "icon-name":
							name = "icon-name";
						break;
						case "stock":
							name = "icon-stock";
						break;
						case "file":
							name = "icon-file";
						break;
					}
				}
				this.set(name, val);
			}
			private void item_label_set(Gtk.Widget w, Gtk.Widget? l) {
				weak Gtk.Label old_label = (Gtk.Label) w.get_data("old-label");
				if(l != old_label) {
					if(old_label != null) {
						set_native_name(old_label, null);
						/*
						old_label.notify["label"] -= item_property_notify;
						*/
						old_label.notify -= item_property_notify;
						old_label.notify -= accel_label_notify;
					}
					if(l!= null) {
						l.notify["label"] += item_property_notify;
						this.set("label", (l as Gtk.Label).label);
						w.set_data_full("old-label", l.ref(), g_object_unref);
						if(l is Gtk.AccelLabel) {
							l.notify["accel-widget"] += accel_label_notify;
							l.notify["accel-closure"] += accel_label_notify;
							accel_label_notify(l as Gtk.AccelLabel);
						}
						if(l is Gtk.Separator || l is Gtk.SeparatorMenuItem) {
							set("label", "|");
						}
					} else  {
						set("label", "|");
						w.set_data("old-label", null);
					}
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
						connect_to_image(image);
						item.set_data_full("old-image", image.ref(), g_object_unref);
					} else {
						item.set_data("old-image", null);
					}
				}
			}
			private void accel_label_notify(Gtk.Widget gtk) {
				(gtk as Gtk.AccelLabel).refetch();
				string s = (gtk as Gtk.AccelLabel).accel_string;
				weak string trimmed = s.strip();	
				if(trimmed.size() >0) set("accel", trimmed);
			}
		}
		public Document() {}
		private static int unique = 999;
		public GMarkup.Tag CreateTag(string tag) {
			Tag rt = new Widget(this);
			rt.tag = tag;
			rt.name = S("WIDGET" + unique.to_string());
			unique++;
			return rt;
		}
		public weak Document.Widget wrap(Gtk.Widget gtk) {
			weak string name = get_native_name(gtk);
			if(name != null) {
				weak Document.Widget rt = dict.lookup(name) as Document.Widget;
				if(rt != null) return rt;
			}
			int id = Client.instance().unique;
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
		private static void set_native_name(void* w, string? name) {
			((GLibCompat.constpointer)w).set_data("native-name", (void*) name);
		}
		private static weak Gtk.Widget? find_menu_item_label(Gtk.Widget widget) {
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
					return w;
				}
				if(w is Gtk.Separator || w is Gtk.SeparatorMenuItem) return w;
			}
			return null;
		}
	}
}
