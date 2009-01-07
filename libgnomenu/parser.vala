using Gtk;

namespace Gnomenu {
	/**
	 * Parser converts xml to widgets.
	 *
	 * A sub-parser is created if a sumne is encountered.
	 */
	public class Parser {
		public static void parse(MenuBar menubar, string description) throws GLib.Error {
			var parser = new Parser();
			var timer = new Timer();
			parser.shell = menubar;
			parser.topmost = menubar;
			MarkupParseContext context = 
				new MarkupParseContext(parser.functions, 0, parser, null);
			context.parse(description, -1);
			message("Parser consumed: %lf for %ld bytes", timer.elapsed(null),
					description.size());
		}

		Parser () {
			position = 0; 
			inside_item = false;
			this.shell = null;
			this.topmost = null;
			MarkupParser parser_funcs = {
					start_element,
					end_element,
					null, null, null
			};
			Memory.copy(&this.functions, &parser_funcs, sizeof(MarkupParser));
		}
		MarkupParser functions;
		MenuShell shell;
		MenuBar topmost;
		MenuItem item;

		int position;
		bool inside_item;
		bool item_has_submenu;
		Parser child_parser; /*to hold the ref count*/

		private void start_element (MarkupParseContext context, 
				string element_name, 
				string[] attribute_names, 
				string[] attribute_values) throws MarkupError {
			switch(element_name){
				case "menu":
					if(inside_item == false) {
						/*Ignore it, allowing 
						 * <menu> <item/><item/><item/> </menu>
						 */
					} else {
		//				weak MenuItem item = gtk_menu_shell_get_item(shell, position) as MenuItem;
						if(item.submenu == null) {
							item.submenu = (Menu) item.get_data("_saved_menu_");
						}	
						child_parser = new Parser();
						child_parser.shell = item.submenu;
						child_parser.topmost = this.topmost;
						g_markup_parse_context_push(context, functions, child_parser);
						item_has_submenu = true;
					}
				break;
				case "item":
					/*NOTE: after the first time we has(position) == false,
					 * it should be false forever)*/
					item = gtk_menu_shell_get_item(shell, position) as MenuItem;
					if(item != null) {
						setup_item(item, attribute_names, attribute_values);
					} else {
						item = new MenuItem();
						item.set_data_full("_saved_menu_", (new Menu()).ref(), g_object_unref);
						shell.append(item);
						item.position = position;
						item.menubar = topmost;
						setup_item(item, attribute_names, attribute_values);
					}
					inside_item = true;
					item_has_submenu = false;
				break;
				default:
					throw new MarkupError.UNKNOWN_ELEMENT("unkown element");
			}
		}
		private void setup_item(MenuItem item, 
				string[] attr_names, 
				string[] attr_vals) {
			weak string label = null;
			weak string icon = null;
			weak string type = null;
			weak string state = null;
			weak string font = null;
			weak string id = null;
			weak string accel = null;
			weak bool sensitive = true;
			weak bool visible = true;
			g_markup_collect_attributes("item", attr_names, attr_vals, null,
					GMarkupCollectType.STRING | GMarkupCollectType.OPTIONAL,
					"label", &label, 
					GMarkupCollectType.STRING | GMarkupCollectType.OPTIONAL,
					"type", &type, 
					GMarkupCollectType.STRING | GMarkupCollectType.OPTIONAL,
					"state", &state, 
					GMarkupCollectType.STRING | GMarkupCollectType.OPTIONAL,
					"font", &font, 
					GMarkupCollectType.STRING | GMarkupCollectType.OPTIONAL,
					"id", &id, 
					GMarkupCollectType.STRING | GMarkupCollectType.OPTIONAL,
					"icon", &icon, 
					GMarkupCollectType.STRING | GMarkupCollectType.OPTIONAL,
					"accel", &accel, 
					GMarkupCollectType.TRISTATE,
					"visible", &visible, 
					GMarkupCollectType.TRISTATE,
					"sensitive", &sensitive, 
					GMarkupCollectType.INVALID
					);
			if(visible != false)
				visible = true;
			
			if(sensitive != false)
				sensitive = true;

			item.truncated = false;
			item.id = id;
			item.accel_text = accel;
			item.visible = visible;
			item.sensitive = sensitive;
			item.item_type = type;
			item.label = label;
			item.icon= icon;
			item.item_state = state;
			item.font = font;
		}
		private void end_element (MarkupParseContext context, 
				string element_name) throws MarkupError {
			switch(element_name) {
				case "menu":
					if(inside_item) {
						/* stop the child parser */
						g_markup_parse_context_pop(context);
						gtk_menu_shell_truncate(child_parser.shell, child_parser.position);
						child_parser = null;
					} else {
						gtk_menu_shell_truncate(shell, position);
					}
					break;
				case "item":
					if(!item_has_submenu) {
		//				var item = gtk_menu_shell_get_item(shell, position) as MenuItem;
						item.submenu = null;
					}
					inside_item = false;
					position++;
				break;
			}
		}
	}
}
