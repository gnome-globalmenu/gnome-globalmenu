using GLib;
using Gtk;
using GtkCompat;
using Gnomenu;
using GMarkup;
using DBus;
namespace Gnomenu {
	public class MenuBar : Gtk.Notebook {
		private RemoteDocument serverdoc;
		private DBus.Connection conn;
		private HashTable<string, Gtk.Widget> menu_hash;
		private HashTable<string, string> bus_hash;
		public MenuBar() {
		}
		private void remove_page_by_xid(string xid) {
			weak Gtk.Widget view = menu_hash.lookup(xid);
			int num = (this as GtkCompat.Notebook).page_num(view);
			if(num >= 0) {
				this.remove_page(num);
			}
			menu_hash.remove(xid);
		}
		private weak string? find_default() {
			foreach(weak GMarkup.Node node in serverdoc.root.children) {
				if(node is GMarkup.Tag)
				if((node as GMarkup.Tag).get("default") == "true") {
					debug("Default is found");
					return (node as GMarkup.Tag).name;
				}
			}
			return null;
		}
		public void switch(string xid) {
			bool need_new_menu_view = false;
			weak GMarkup.Tag node = serverdoc.dict.lookup(xid) as GMarkup.Tag;
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
				dynamic DBus.Object client;
				RemoteDocument clientdoc = RemoteDocument.connect(bus, "/org/gnome/GlobalMenu/Application");
				client = conn.get_object(bus, "/org/gnome/GlobalMenu/Application", "org.gnome.GlobalMenu.Client");
				string widget_name = client.QueryXID(xid);
				debug("widget_name %s", widget_name);
				node = clientdoc.dict.lookup(widget_name) as GMarkup.Tag;
				if(node != null) {
					Gtk.Box box = new Gtk.VBox(false, 0);
					box.visible = true;
					(box as GtkCompat.Widget).style_set += (box, style) => {
						foreach (weak Gtk.Widget child in (box as Gtk.Container).get_children()) {
							child.set_style((box as Gtk.Widget).style);
						}
					};
					foreach(weak GMarkup.Node c in node.children) {
						if(!(c is GMarkup.Tag)) continue;
						if((c as GMarkup.Tag).tag == "menubar") {
							MenuView view = new MenuView();
							view.visible = true;
							debug("menubar found");
							GMarkup.Section section = new GMarkup.Section(clientdoc, c);
							view.document = section;
							view.set_data("xid", view.document.S(xid));
							view.set_data_full("client", client.ref(), g_object_unref);
							view.activated += (view, node) => {
								dynamic DBus.Object client = (DBus.Object) view.get_data("client");
								weak string xid = (string) view.get_data("xid");
								message("shit %s", xid);
								try {
									client.Activate(xid, node.name);
								} catch (GLib.Error e){
									warning("%s", e.message);
								}
							};
							box.pack_start(view, true, true, 0);
						}
					}
					this.remove_page_by_xid(xid);
					box.set_style(this.style);
					this.append_page(box, null);
					menu_hash.insert(xid, box);
				}
			}
			int num = (this as GtkCompat.Notebook).page_num(menu_hash.lookup(xid));
			if(num != -1) this.set_current_page(num);
			else this.set_current_page(0);
		}
		construct {
			serverdoc = RemoteDocument.connect("org.gnome.GlobalMenu.Server", "/org/gnome/GlobalMenu/Server");
			conn = Bus.get(DBus.BusType.SESSION);
			menu_hash = new HashTable<string, Gtk.Widget>.full(str_hash, str_equal, g_free, g_object_unref);
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
			box.pack_start(entry, false, true, 0);
			box.pack_start(button, false, true, 0);
			box.pack_start(menubar, true, true, 0);
			window.show_all();
			loop.run();
			return 0;
		}
	}
}
