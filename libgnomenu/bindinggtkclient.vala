using GLib;
using DBus;
using Gnomenu;
using XML;
using GtkAQD;

namespace GnomenuGtk {
	protected class GtkNodeFactory : Client.NodeFactory {
		HashTable<weak string, weak Gtk.Widget> dict_nw;
		HashTable<weak string, weak TagNode> dict_nn;
		private Gtk.TreeStore tree;
		private class TagNode:XML.TagNode {
			public Gtk.TreeIter iter;
			public TagNode(NodeFactory factory) {
				this.factory = factory;
			}
		}
		private class WidgetNode:TagNode {
			public WidgetNode(NodeFactory factory) {
				this.factory = factory;
			}
			~WidgetNode() {
				message("XMLNode %s is removed", this.get("name"));
				(this.factory as GtkNodeFactory).dict_nn.remove(this.get("name"));
			}
		}
		public GtkNodeFactory() {}
		construct {
			dict_nw = new HashTable<weak string, weak Gtk.Widget>(str_hash, str_equal);
			dict_nn = new HashTable<weak string, weak Gtk.Widget>(str_hash, str_equal);
			tree = new Gtk.TreeStore(1, typeof(string));
		}
		public override weak XML.TagNode? lookup(string name) {
			return dict_nn.lookup(name);
		}
		public override XML.TagNode CreateWidgetNode(string name) {
			weak TagNode node = dict_nn.lookup(name);
			if(node != null) return node;
			TagNode rt = new WidgetNode(this);
			weak Gtk.Widget gtk = dict_nw.lookup(name);
			assert(gtk != null);
			if(gtk is Gtk.MenuItem) { 
				rt.tag = "item";
			}
			if(gtk is Gtk.MenuShell) {
				rt.tag = "menu";
			}
			if(gtk is Gtk.Window) {
				rt.tag = "window";
			}
			rt.set("name", name);
			dict_nn.insert(name, rt);
			return rt;
		}
		public override void FinishNode(XML.Node node) {}
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
					assert(node.parent != null);
					node.parent.remove(node);
				}
			}
			object.set_data("native-name", null);
			object_remove_toggle_ref(object, toggle_ref_notify, this);
		}
	}

}
