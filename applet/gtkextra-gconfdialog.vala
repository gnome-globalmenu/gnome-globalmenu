/*******************************
 * GConfDialog
 * Author: Luca Viggiani
 * Version: 0.1
 * Date: 2009-12-07
 * License: GPL
 * Description: creates a gconf settings back end dialog 
 * 				automatically from a settings key root.
 * Usage: new GConfDialog (string key, string dialog_title)
 * Params:
 * 	    string key: the gconf key to connect to (i.e /apps/panel/applets/applet_46/prefs)
 * 		string dislog_title: text to be shown on window title bar
 *		string[] subkeys: used to choose the subkeys to hadle (comprising display order).
 * 			  if null all subkeys will be retieved.
 * 
 * TODO: Only BOOL and INT data types are implemented. STRING is implemented only partially (read)
 *******************************/

using GLib;
using Gtk;
using GConf;

	public class GConfDialog : Gtk.Dialog {

		private GConf.Client _default_client;

		public GConfDialog(string dialog_title) {
			title = dialog_title;
			icon_name = "gtk-preferences";
		}

		public GConfDialog.with_subkeys(string key, string dialog_title, string[]? subkeys = null) {
			title = dialog_title;
			icon_name = "gtk-preferences";
			add_subkeys(key, subkeys);
		}
		construct {
			_default_client = GConf.Client.get_default();
			//vbox.width_request = 320;
		}
		public void add_key(string key) {
			weak GConf.Entry entry = _default_client.get_entry(key, null, true);
			if(entry != null)
				addEntry(entry);
		}
		public void add_subkeys(string key, string[]? subkeys= null) {
			weak GLib.SList<GConf.Entry> prefs = _default_client.all_entries(key);
			if (subkeys == null) {
				foreach(weak GConf.Entry entry in prefs)
					addEntry(entry);
       		} else {
				foreach(weak string subkey in subkeys) {
					add_key(key + "/" + subkey);
				}
			}
		}

		private void addEntry(GConf.Entry entry) {
			Gtk.Box row = new Gtk.HBox(false, 0);
       		string schema_name = entry.get_schema_name();
       		weak GConfCompat.Schema schema = (GConfCompat.Schema)_default_client.get_schema(schema_name);
       			
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
       				(widget as Gtk.CheckButton).active = _default_client.get_bool(entry.key);
       				(widget as Gtk.CheckButton).clicked += onCheckButtonActivated;
       				break;
       			case ValueType.STRING:
       				widget = new Gtk.Entry();
       				(widget as Gtk.Entry).text = _default_client.get_string(entry.key);
       				/* TODO: connect changed signal to eventhandler */
       				break;
       			case ValueType.INT:
       				widget = new Gtk.SpinButton.with_range(-100, 100, 1);
       				(widget as Gtk.SpinButton).value = _default_client.get_int(entry.key);
       				(widget as Gtk.SpinButton).value_changed += onSpinButtonValueChanged;
       				break;
       			default:
       				return;
       				break;
			}
			widget.user_data = entry;
				
			row.pack_start(new Gtk.EventBox(), true, true, 2);
			row.pack_start(widget, false, false, 2);
				
			Gtk.Button button = new Gtk.Button();
			button.tooltip_text = "Resets to default value";
			button.set_image(new Gtk.Image.from_stock("gtk-clear", Gtk.IconSize.BUTTON));
			button.user_data = widget;
			button.clicked += onResetButtonPressed;
				
			row.pack_start(button, false, false, 2);
				
       		row.show_all();
       		vbox.pack_start(row, true, true, 2);
		}
		private void onCheckButtonActivated(Gtk.CheckButton widget) {
			weak GConf.Entry entry = (GConf.Entry)widget.user_data;
			_default_client.set_bool(entry.key, widget.active);
		}
		
		private void onSpinButtonValueChanged(Gtk.SpinButton widget) {
			weak GConf.Entry entry = (GConf.Entry)widget.user_data;
			_default_client.set_int(entry.key, (int)widget.value);
		}
		
		private void onResetButtonPressed(Gtk.Button widget) {
			Gtk.Widget target = (Gtk.Widget)widget.user_data;
			weak GConf.Entry entry = (GConf.Entry)target.user_data;
			string schema_name = entry.get_schema_name();
			weak GConfCompat.Schema schema = (GConfCompat.Schema)_default_client.get_schema(schema_name);
			switch(schema.get_type()) {
       			case ValueType.BOOL:
       				(target as Gtk.CheckButton).active = schema.get_default_value().get_bool();
       				break;
       			case ValueType.STRING:
       				(target as Gtk.Entry).text = schema.get_default_value().get_string();
       				break;
       			case ValueType.INT:
       				(target as Gtk.SpinButton).value = schema.get_default_value().get_int();
       				break;
				}
		}
	}
