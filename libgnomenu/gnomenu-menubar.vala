using GLib;
using Gtk;
using GtkCompat;
using Gnomenu;
using GMarkupDoc;
using DBus;
namespace Gnomenu {
	public class MenuBar : Gtk.Notebook {
		private RemoteDocument serverdoc;
		private DBus.Connection conn;
		private HashTable<string, MenuView> menu_hash;
		private HashTable<string, string> bus_hash;
		public MenuBar() {
		}
		private void remove_page_by_xid(string xid) {
			weak MenuView view = menu_hash.lookup(xid);
			int num = (this as GtkCompat.Notebook).page_num(view);
			if(num >= 0) {
				this.remove_page(num);
			}
			menu_hash.remove(xid);
		}
		private weak string? find_default() {
			foreach(weak GMarkupDoc.Node node in serverdoc.root.children) {
				if(node is GMarkupDoc.Tag)
				if((node as GMarkupDoc.Tag).get("default") == "true") {
					debug("Default is found");
					return (node as GMarkupDoc.Tag).name;
				}
			}
			return null;
		}
		public void switch(string xid) {
			bool need_new_menu_view = false;
			weak GMarkupDoc.Tag node = serverdoc.dict.lookup(xid) as GMarkupDoc.Tag;
			if(node == null) {
				this.remove_page_by_xid(xid);
				bus_hash.remove(xid);
				this.set_current_page(0);
			}
			if(node == null /* try default window*/) {
				weak string xid = find_default();
				if(xid != null) {
					@switch(xid);
					return;
				}
			}
			if(node == null /*try transient for window*/) {
				/*TODO*/
			}
			if(node == null) return;
			weak string bus = node.get("bus");
			debug("XID %s at BUS %s", xid, bus);
			
			if(bus_hash.lookup(xid) != bus) {
				bus_hash.insert(xid, bus);
				need_new_menu_view = true;
				debug("BUS changed for XID");
			}
			if(menu_hash.lookup(xid) == null) {
				debug("No menuview for this XID");
				need_new_menu_view = true;
			}
			if(need_new_menu_view) {
				RemoteDocument clientdoc = new RemoteDocument(bus, "/org/gnome/GlobalMenu/Application");
				dynamic DBus.Object client = conn.get_object(bus, "/org/gnome/GlobalMenu/Application", "org.gnome.GlobalMenu.Client");
				string widget_name = client.QueryXID(xid);
				debug("widget_name %s", widget_name);
				node = clientdoc.dict.lookup(widget_name) as GMarkupDoc.Tag;
				if(node != null) {
					MenuView view = new MenuView(null);
					view.visible = true;
					foreach(weak GMarkupDoc.Node c in node.children) {
						if(!(c is GMarkupDoc.Tag)) continue;
						if((c as GMarkupDoc.Tag).tag == "menubar") {
							debug("menubar found");
							GMarkupDoc.Section section = new GMarkupDoc.Section(clientdoc, c);
							view.document = section;
						}
					}
					this.remove_page_by_xid(xid);
					view.set_style(this.style);
					this.append_page(view, null);
					menu_hash.insert(xid, view);
				}
			}
			int num = (this as GtkCompat.Notebook).page_num(menu_hash.lookup(xid));
			if(num != -1) this.set_current_page(num);
			else this.set_current_page(0);
		}
		construct {
			serverdoc = new RemoteDocument("org.gnome.GlobalMenu.Server", "/org/gnome/GlobalMenu/Server");
			conn = Bus.get(DBus.BusType.SESSION);
			menu_hash = new HashTable<string, MenuView>.full(str_hash, str_equal, g_free, g_object_unref);
			bus_hash = new HashTable<string, string>.full(str_hash, str_equal, g_free, g_free);
			Gtk.EventBox dummy = new Gtk.EventBox();
			dummy.add(new Gtk.Label("no menu"));
			dummy.set_style(this.style);
			this.append_page(dummy, null);
			this.show_border = false;
			(this as GtkCompat.Widget).style_set += (widget, style)=> {
				int n = this.get_n_pages();
				for(int i = 0; i<n ;i++) {
					this.get_nth_page(i).set_style(this.style);
				}
			};
			int n = this.get_n_pages();
			for(int i = 0; i<n ;i++) {
				this.get_nth_page(i).set_style(this.style);
			}
		}
		public static int test(string[] args) {
			Gtk.init(ref args);
			MainLoop loop = new MainLoop(null, false);
			Gtk.Window window = new Gtk.Window(WindowType.TOPLEVEL);
			MenuBar menubar = new MenuBar();
			Gtk.Box box = new Gtk.HBox(false, 0);
			Gtk.Entry entry = new Gtk.Entry();
			Gtk.Button button = new Gtk.Button.with_label("Go");
			entry.set_text(args[1]);
			button.set_data("menu-bar", menubar);
			button.set_data("entry", entry);
			button.clicked += (button) => {
				MenuBar menubar = (MenuBar) button.get_data("menu-bar");
				Gtk.Entry entry = (Gtk.Entry) button.get_data("entry");
				menubar.switch(entry.get_text());
			};
			window.add(box);
			box.pack_start_defaults(entry);
			box.pack_start_defaults(button);
			box.pack_start_defaults(menubar);
			window.show_all();
			loop.run();
			return 0;
		}
	}
}
