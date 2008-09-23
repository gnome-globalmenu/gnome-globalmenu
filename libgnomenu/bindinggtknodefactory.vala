using GLib;
using DBus;
using Gnomenu;
using XML;
using GtkAQD;

namespace GnomenuGtk {
	protected class NodeFactory : Client.NodeFactory {
		HashTable<weak string, weak Gtk.Widget> dict_nw;
		HashTable<weak string, weak TagNode> dict_nn;
		public Gtk.TreeStore tree;
		private class TagNode:XML.TagNode {
			public Gtk.TreeIter iter;
			public TagNode(NodeFactory factory) {
				this.factory = factory;
			}
			~TagNode() {
				message("TagNode %s is removed", this.tag);
				(this.factory as NodeFactory).tree.remove(this.iter);
			}
		}
		private class WidgetNode:TagNode {
			public WidgetNode(NodeFactory factory) {
				this.factory = factory;
			}
			~WidgetNode(){
				message("WidgetNode %s is removed", this.get("name"));
				(this.factory as NodeFactory).dict_nn.remove(this.get("name"));
			}
		}
		public NodeFactory() {}
		construct {
			dict_nw = new HashTable<weak string, weak Gtk.Widget>(str_hash, str_equal);
			dict_nn = new HashTable<weak string, weak Gtk.Widget>(str_hash, str_equal);
			tree = new Gtk.TreeStore(1, typeof(constpointer));
		}
		public override weak XML.TagNode? lookup(string name) {
			return dict_nn.lookup(name);
		}
		public override XML.TagNode CreateWidgetNode(string name) {
			weak TagNode node = dict_nn.lookup(name);
			if(node != null) return node;
			TagNode rt = new WidgetNode(this);
			rt.freeze();
			weak Gtk.Widget gtk = dict_nw.lookup(name);
			assert(gtk != null);
			rt.set("name", name);
			if(gtk is Gtk.MenuItem) { 
				rt.tag = "item";
				if(gtk is Gtk.TearoffMenuItem) {
					rt.set("title", "&");
				} else 
				if(gtk is Gtk.SeparatorMenuItem) {
					rt.set("title", "|");
				} else  {
					weak Gtk.Label l = find_menu_item_label(gtk);
					if(l!= null) {
						rt.set("title", l.label);
					}
					if(gtk is Gtk.CheckMenuItem) {
						rt.set("active", (gtk as Gtk.CheckMenuItem).active?"true":"false");
						rt.set("draw-as-radio", (gtk as Gtk.CheckMenuItem).draw_as_radio?"true":"false");
						rt.set("inconsistent", (gtk as Gtk.CheckMenuItem).inconsistent?"true":"false");
					}
				}
			}
			if(gtk is Gtk.MenuShell) {
				rt.tag = "menu";
			}
			if(gtk is Gtk.Window) {
				rt.tag = "window";
			}
			dict_nn.insert(name, rt);
			return rt;
		}
		public override void FinishNode(XML.Node node) {
			if(node is TagNode) {
				if(node.parent is TagNode) {
					tree.insert(out (node as TagNode).iter, (node.parent as TagNode).iter, node.parent.index(node));
				} else {
					tree.insert(out (node as TagNode).iter, null, 0);
				}
				tree.set((node as TagNode).iter, 0, node, -1);
			}
			node.unfreeze();
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
			weak string name = (string) object.get_data("native-name");
			if(name != null) {
				message("GtkWidget %s is removed", name);
				dict_nw.remove(name); // because ~WidgetNode is not always invoked?
				weak TagNode node = dict_nn.lookup(name);
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
