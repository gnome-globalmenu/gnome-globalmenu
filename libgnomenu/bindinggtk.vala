using GLib;
using Gdk;
using Gtk;
using DBus;
using Markup; 
using Gnomenu;
using GtkAQD;

namespace GnomenuGtk {
	public HashTable <string, weak Gtk.Widget> widgets;
	public StringChunk strings;
	public int id;
	public Client client;
	public void bind_menu(Gtk.Widget window, Gtk.Widget widget) {
		client.add_menu(get_native_widget(window), get_native_widget(widget));
	}
	public void unbind_menu(Gtk.Widget window, Gtk.Widget widget) {
		client.remove_menu(get_native_widget(window), get_native_widget(widget));
	}
	private void toggle_ref_notify(void* data, GLib.Object object, bool is_last){
		if(is_last) {
			unwrap_widget(object as Gtk.Widget);
		}
	}
	private weak string? get_native_widget(Gtk.Widget widget) {
		weak string rt = (string) widget.get_data("native-menu-object");
		if(rt == null) {
			wrap_widget(widget);
		}
		rt = (string) widget.get_data("native-menu-object");
		return rt;
	}
	private weak Gtk.Widget? get_gtk_widget(string widget){
		return (Gtk.Widget)widgets.lookup(widget);
	}
	private void handle_menu_item_label(Gtk.Widget widget) {
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
				w.set_data("native-menu-object", get_native_widget(widget));
				w.notify += wrap_widget_widget_updated;
				widget.set_data_full("label", w.ref(), 
						(o) => {
							weak Gtk.Widget w = (Gtk.Widget) o; 
							w.notify -= wrap_widget_widget_updated;
							w.unref();
						});
				break;
			}
		}
	}
	private void wrap_widget_register_window(Gtk.Widget window) {
		Gdk.Window gdk_window = window.window;
		ulong xwindow = Gdk.XWINDOW(gdk_window);
		client.register_window(get_native_widget(window), xwindow.to_string());
	}
	private void wrap_widget_unregister_window(Gtk.Widget window) {
		client.unregister_window(get_native_widget(window));
	}
	private void wrap_widget_widget_updated(Gtk.Widget widget) {
		client.sync_widget(get_native_widget(widget));
		client.updated(get_native_widget(widget));
	}
	public void wrap_widget(Gtk.Widget widget) {
		weak string name = (string) widget.get_data("native-menu-object");
		if(name != null) {
			return;
		}
		name = strings.insert_const("%s%d".printf(widget.get_type().name(), id));
		id++;
		widget.set_data("native-menu-object", name);
		object_add_toggle_ref(widget, toggle_ref_notify, name);
		widgets.insert(name, widget);
		if(widget is Gtk.Window) {
			client.add_window(name);
			widget.realize += wrap_widget_register_window;
			widget.unrealize += wrap_widget_unregister_window;
		}
		if(widget is GtkAQD.MenuShell) {
			(widget as GtkAQD.MenuShell).add += wrap_widget_widget_updated;
			(widget as GtkAQD.MenuShell).insert += wrap_widget_widget_updated;
			(widget as GtkAQD.MenuShell).remove += wrap_widget_widget_updated;
		}
		if(widget is Gtk.MenuItem) {
			handle_menu_item_label(widget);
			widget.notify += wrap_widget_widget_updated;
		}
	}
	private void unwrap_widget(Gtk.Widget widget) {
		weak string name = (string) widget.get_data("native-menu-object");
		if(name != null) {
			if(widget is Gtk.Window) {
				widget.realize -= wrap_widget_register_window;
				widget.unrealize -= wrap_widget_unregister_window;
				client.remove_window(name);
			}
			if(widget is Gtk.MenuItem) {
				widget.notify -= wrap_widget_widget_updated;
			}
			if(widget is GtkAQD.MenuShell) {
				(widget as GtkAQD.MenuShell).add -= wrap_widget_widget_updated;
				(widget as GtkAQD.MenuShell).insert -= wrap_widget_widget_updated;
				(widget as GtkAQD.MenuShell).remove -= wrap_widget_widget_updated;
			}
			if(widget is Gtk.CheckMenuItem) {
				(widget as Gtk.CheckMenuItem).toggled -= wrap_widget_widget_updated;
			}
			object_remove_toggle_ref(widget, toggle_ref_notify, name);
			widgets.remove(name);
		}
	}
	/** invoke after gtk_init, and after the main loop is ready **/
	public static void init_args([CCode (arry_length_pos = 0.9)] ref weak string[] args) {
		init();
	}
	/** cont. Better to invoke when the first window is realized or somewhere...**/
	public static void init() {
		if(client!= null) return;
		client = new Client();
		id = 99;
		widgets = new HashTable <weak string, weak Gtk.Widget> (str_hash, str_equal);
		strings = new StringChunk(1024);
	}
	public class Client: Gnomenu.Client {
		public Client() { }
		public static int test(string[] args) {
			Gtk.init(ref args);
			MainLoop loop = new MainLoop(null, false);
			init();
			Gtk.Widget window = new Gtk.Window(Gtk.WindowType.TOPLEVEL);
			Gtk.Widget menu = new Gtk.MenuBar();
			bind_menu(window as Gtk.Window, menu);
			window.show_all();
			loop.run();
			return 0;
		}
		private Gnomenu.Client.XMLWidgetNode visit_gtk_widget(Gtk.Widget widget){
			string type = "unknown";
			if(widget is Gtk.MenuItem) {
				type = "item";
			}
			if(widget is Gtk.MenuShell) {
				type = "menu";
			}	
			Gnomenu.Client.XMLWidgetNode node = new
				Gnomenu.Client.XMLWidgetNode(xml.S(type),
						xml.S(get_native_widget(widget)),
						dict);
			sync_node(node);
			return node;
		}
		public void sync_widget(string widget) {
			weak Gnomenu.Client.XMLWidgetNode node = dict.lookup(widget);
			if(node != null) {
				if(!(node is Gnomenu.Client.XMLWidgetNode)) {
					warning("%s is no longer a node!", widget);
					warning("It happens because VALA doesn't invoke the deconstructor of XMLWidgetNode");
					return;
				}
				sync_node(node);
			}
		}
		private override void sync_node(Gnomenu.Client.XMLWidgetNode node) {
			weak Gtk.Widget widget = get_gtk_widget(node.widget);
			if(widget is Gtk.MenuItem) {
				Gtk.MenuItem item = widget as Gtk.MenuItem;
				handle_menu_item_label(widget);
				weak Gtk.Label label = (Gtk.Label) widget.get_data("label");
				if(label != null) {
					node.set(xml.S("label"), label.label);
					if(label is Gtk.AccelLabel) {
						weak Gtk.AccelLabel accel_label = label as Gtk.AccelLabel;
						accel_label.refetch();
						if(accel_label.accel_string.strip().size() > 0)
							node.set(xml.S("accel"), accel_label.accel_string);
					}
				} else {
					if(widget is Gtk.SeparatorMenuItem) {
						node.set(xml.S("label"), "|");
					} else
					if(widget is Gtk.TearoffMenuItem) {
						node.set(xml.S("label"), "&");
					} else
					node.set(xml.S("label"), get_native_widget(widget));
				}
				if(item.submenu != null) {
					node.children.append(visit_gtk_widget(item.submenu));
				}
			}
			if(widget is Gtk.CheckMenuItem) {
				weak Gtk.CheckMenuItem item = widget as Gtk.CheckMenuItem;
				node.set(xml.S("active"), item.active?"true":"false");
				node.set(xml.S("draw-as-radio"), item.draw_as_radio?"true":"false");
				node.set(xml.S("inconsistent"), item.inconsistent?"true":"false");
			}
			if(widget is Gtk.MenuShell) {
				node.children = null;
				weak List<weak Gtk.Widget> children = (widget as Gtk.Container).get_children();
				foreach (weak Gtk.Widget child in children){
					node.children.append(visit_gtk_widget(child));
				}
			}	
		}
		private override void activate_item(Gnomenu.Client.XMLWidgetNode item_node) {
			weak Gtk.Widget widget = get_gtk_widget(item_node.widget);
			if(widget is Gtk.MenuItem) {
				(widget as Gtk.MenuItem).activate();
			}
		}
	}


}
