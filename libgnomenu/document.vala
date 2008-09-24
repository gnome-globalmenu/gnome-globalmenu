using GLib;
using Gtk;
using XML;
namespace Gnomenu {
	public abstract class Document: XML.Document {
		public abstract class Widget:XML.Document.Tag {
			public weak string name {
				get {return get("name");}
				set {
					if(name != null)
					(document as Document).dict.remove(name);
					set("name", value);
					if(name != null)
					(document as Document).dict.insert(name, this);
				}
			}
			public Widget(Document document) {
				this.document = document;
			}
			construct {
			}
			public override void dispose() {
				(document as Document).dict.remove(name);
			}
			~Widget(){
				message("WidgetNode %s is removed", name);
			}
			public abstract virtual void activate();
		}
		public abstract virtual Widget CreateWidget(string type, string name);
		private HashTable <weak string, weak Widget> dict;
		construct {
			dict = new HashTable<weak string, weak XML.Document.Tag>(str_hash, str_equal);
		}
		public virtual weak Widget? lookup(string name) {
			return dict.lookup(name);
		}
		public override void FinishNode(XML.Node node) {
			node.unfreeze();
		}
	}
}
