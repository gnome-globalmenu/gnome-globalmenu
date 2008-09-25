using GLib;
using DBus;
using Gnomenu;
using XML;
using GtkAQD;

namespace GnomenuGtk {
	protected class Document : Gnomenu.Document {
		HashTable<weak string, weak Gtk.Widget> dict_nw;
		public Gtk.TreeStore tree;
		private class Widget:Gnomenu.Document.Widget {
			public Gtk.TreeIter iter;
			public Widget(Document document) {
				this.document = document;
			}
			public override void dispose() {
				base.dispose();
				(this.document as Document).tree.remove(this.iter);
			}
			public override void activate() {
				weak Gtk.Widget widget = (document as Document).dict_nw.lookup(this.get("name"));
				if(widget is Gtk.MenuItem) (widget as Gtk.MenuItem).activate();
				if(widget is GtkAQD.MenuBar) {
					bool local = (widget as GtkAQD.MenuBar).local;
					(widget as GtkAQD.MenuBar).local = !local;
				}
			}
		}
		public Document() {}
		construct {
			dict_nw = new HashTable<weak string, weak Gtk.Widget>(str_hash, str_equal);
			tree = new Gtk.TreeStore(1, typeof(constpointer));
		}
		public override XML.Document.Text CreateText(string text) {
			XML.Document.Text rt = new XML.Document.Text(this);
			rt.freeze();
			rt.text = text;
			return rt;
		}
		public override  XML.Document.Special CreateSpecial(string text) {
			XML.Document.Special rt = new XML.Document.Special(this);
			rt.freeze();
			rt.text = text;
			return rt;
		}
		public override XML.Document.Tag CreateTag(string tag) {
			XML.Document.Tag rt = new XML.Document.Tag(this);
			rt.freeze();
			rt.tag = S(tag);
			return rt;
		}
		public override Gnomenu.Document.Widget CreateWidget(string type, string name) {
			weak Gnomenu.Document.Widget node = lookup(name);
			if(node != null) return node;
			Widget rt = new Widget(this);
			rt.freeze();
			weak Gtk.Widget gtk = dict_nw.lookup(name);
			rt.name = name;
			rt.tag = type;
			return rt;
		}
		public override void FinishNode(XML.Node n) {
			if(n is Widget) {
				message("FinishNode Widget");
				weak Widget node = n as Widget;
				if(node.parent is Widget) {
					tree.insert(out node.iter, (node.parent as Widget).iter, node.parent.index(node));
				} else {
					tree.insert(out node.iter, null, 0);
				}
				tree.set(node.iter, 0, node, -1);
				weak Gtk.Widget gtk = dict_nw.lookup(node.get("name"));
				if(gtk is Gtk.MenuItem) { 
					refresh_item_property(gtk, "visible");
					gtk.notify["visible"] += item_property_notify;
					refresh_item_property(gtk, "enabled");
					gtk.notify["enabled"] += item_property_notify;
					if(gtk is Gtk.TearoffMenuItem) {
						node.set("label", "&");
					} else 
					if(gtk is Gtk.SeparatorMenuItem) {
						node.set("label", "|");
					} else  {
						weak Gtk.Label l = find_menu_item_label(gtk);
						if(l!= null) {
							refresh_item_property(l, "label");
							l.notify["label"] += item_property_notify;
						}
						if(gtk is Gtk.CheckMenuItem) {
							refresh_item_property(gtk, "active");
							gtk.notify["active"] += item_property_notify;
							refresh_item_property(gtk, "draw-as-radio");
							gtk.notify["draw-as-radio"] += item_property_notify;
							refresh_item_property(gtk, "inconsistent");
							gtk.notify["inconsistent"] += item_property_notify;
						}
					}
				}
			}
			base.FinishNode(n);
		}
		public weak string wrap(Gtk.Widget widget) {
			weak string name = (string)widget.get_data("native-name");
			if(name != null) return name;
			int id = Singleton.instance().unique;
			name = S("%s%d".printf(widget.get_type().name(), id));
			widget.set_data("native-name", name);
			object_add_toggle_ref(widget, toggle_ref_notify, this);
			dict_nw.insert(name, widget);
			return name;
		}
		private void toggle_ref_notify(GLib.Object object, bool is_last){
			if(!is_last) return;
			if(object is Gtk.MenuItem) {
				weak Gtk.Label label = find_menu_item_label(object as Gtk.Widget);
				if(label != null) label.notify["label"] -= item_property_notify;
			}
			unbind_widget(object as Gtk.Widget);
			weak string name = (string) object.get_data("native-name");
			if(name != null) {
				message("GtkWidget %s is removed", name);
				dict_nw.remove(name); // because ~WidgetNode is not always invoked?
				weak Gnomenu.Document.Widget node = lookup(name);
				if(node != null){
					if(node.parent == null) {
						message("parent = null for %s", name);
						assert(false);
					}
					node.parent.remove(node);
				}
			}
			object.set_data("native-name", null);
			object_remove_toggle_ref(object, toggle_ref_notify, this);
		}
		private void refresh_item_property(Gtk.Widget w, string prop) {
			Type t = w.get_type();
			weak TypeClass tc = t.class_peek();
			weak ParamSpec pspec = ((ObjectClass) tc).find_property(prop);
			if(pspec != null) {
				item_property_notify(w, pspec);
			}
		}
		private void item_property_notify(Gtk.Widget w, ParamSpec pspec) {
			weak Gnomenu.Document.Widget node = lookup((string)w.get_data("native-name"));
			if(node == null) {
				warning("no xml node found for widget %s", (string) w.get_data("native-name"));
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
	}
	private weak Gtk.Label? find_menu_item_label(Gtk.Widget widget) {
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
				w.set_data("native-name", widget.get_data("native-name"));
				return w as Gtk.Label;
			}
		}
		return null;
	}
}
