/* GtkExtra.GConfDialog
 * Author: Luca Viggiani
 * Version: 0.1
 * Date: 2009-12-07
 * License: GPL
 * Description: creates a gconf settings back end dialog automatically from a settings key root.
 * 				Usage: new GtkExtra.GConfDialog(string key, string dialog_title)
 * 				Params:
 * 						string key: the gconf key to connect to (i.e /apps/panel/applets/applet_46/prefs)
 * 						string dislog_title: text to be shown on window title bar
 *
 * TODO: Only BOOL and INT data types are implemented. STRING is implemented only partially (read)
 * 		 "Restore default" button for each entry
 */

using GLib;
using Gtk;
using GConf;

namespace GtkExtra {
	public class GConfDialog : Gtk.Dialog {
		private string root_key;
		public GConfDialog(string key, string dialog_title) {
			root_key = key;
			this.title = dialog_title;
			this.set_icon_name("gtk-preferences");
			
			vbox.width_request = 320;
			
       		weak GLib.SList<GConf.Entry> prefs = GConf.Engine.get_default().all_entries(key);
			
       		foreach(weak GConf.Entry entry in prefs) {
       			Gtk.Box row = new Gtk.HBox(false, 0);
       			
       			string schema_name = entry.get_schema_name();
       			weak GConfCompat.Schema schema = (GConfCompat.Schema)Engine.get_default().get_schema(schema_name);
       			
       			Gtk.Image info = new Gtk.Image.from_stock("gtk-dialog-info", Gtk.IconSize.BUTTON);
       			info.tooltip_text = schema.get_long_desc();
       			row.pack_start(info, false, false, 2);
       			
       			Gtk.Label label = new Gtk.Label(schema.get_short_desc());
       			label.justify = Gtk.Justification.LEFT;
       			row.pack_start(label, false, false, 2);
       			
       			Gtk.Widget widget;
       			switch(schema.get_type()) {
       				case ValueType.BOOL:
       					widget = new Gtk.CheckButton();
       					(widget as Gtk.CheckButton).active = Engine.get_default().get_bool(entry.key);
       					(widget as Gtk.CheckButton).clicked += onCheckButtonActivated;
       					break;
       				case ValueType.STRING:
       					widget = new Gtk.Entry();
       					(widget as Gtk.Entry).text = Engine.get_default().get_string(entry.key);
       					/* TODO: connect changed signal to eventhandler */
       					break;
       				case ValueType.INT:
       					widget = new Gtk.SpinButton.with_range(-100, 100, 1);
       					(widget as Gtk.SpinButton).value = Engine.get_default().get_int(entry.key);
       					(widget as Gtk.SpinButton).value_changed += onSpinButtonValueChanged;
       					break;
				}
				widget.user_data = entry.key;
				
				row.pack_start(new Gtk.EventBox(), true, true, 2);
				row.pack_start(widget, false, false, 2);
				
				Gtk.Button button = new Gtk.Button.from_stock("gtk-clear");
				button.tooltip_text = "Resets to default value";
       			vbox.pack_start(row, true, true, 2);
			}
       		
			vbox.show_all();
		}
		
		private void onCheckButtonActivated(Gtk.CheckButton widget) {
			Engine.get_default().set_bool((string)widget.user_data, widget.active);
		}
		
		private void onSpinButtonValueChanged(Gtk.SpinButton widget) {
			Engine.get_default().set_int((string)widget.user_data, (int)widget.value);
		}
	}
}
