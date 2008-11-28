using Gtk;

namespace Gnomenu {
	public class Parser {
		public static void parse(MenuBar menubar, string description) throws GLib.Error {
			var parser = new Parser();
			parser.shell = menubar;
			parser.topmost = menubar;
			MarkupParseContext context = 
				new MarkupParseContext(parser.functions, 0, parser, null);
			context.parse(description, -1);
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
		int position;
		bool inside_item;
		bool item_has_submenu;
		Parser child_parser; /*to hold the ref count*/
		string path;
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
						weak MenuItem item = ((MenuShellHelper)shell).get(position);
						if(item.submenu == null) {
							item.submenu = new Menu();
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
					if(((MenuShellHelper)shell).has(position)) {
						var item = ((MenuShellHelper)shell).get(position);
						setup_item(item, attribute_names, attribute_values);
					} else {
						var item = new MenuItem();
						shell.append(item);
						item.position = position;
						setup_item(item, attribute_names, attribute_values);
						Signal.connect(item, "activate", (GLib.Callback) item_activate_handler, topmost);
					}
					inside_item = true;
					item_has_submenu = false;
				break;
				default:
					throw new MarkupError.UNKNOWN_ELEMENT("unkown element");
			}
		}
		private static void item_activate_handler(MenuItem item, MenuBar topmost) {
			topmost.activate(item);
		}
		private void setup_item(MenuItem item, 
				string[] attr_names, 
				string[] attr_vals) {
			weak string label;
			weak string id;
			g_markup_collect_attributes("item", attr_names, attr_vals, null,
					GMarkupCollectType.STRING | GMarkupCollectType.OPTIONAL,
					"label", &label, 
					
					GMarkupCollectType.INVALID
					);
			item.label = label;
			item.visible = true;
		}
		private void end_element (MarkupParseContext context, 
				string element_name) throws MarkupError {
			switch(element_name) {
				case "menu":
					if(inside_item) {
						/* stop the child parser */
						g_markup_parse_context_pop(context);
						((MenuShellHelper)(child_parser.shell)).truncate(child_parser.position);
						child_parser = null;
					} else {
						((MenuShellHelper)shell).truncate(position);
					}
					break;
				case "item":
					if(!item_has_submenu) {
						var item = ((MenuShellHelper)shell).get(position);
						item.submenu = null;
					}
					inside_item = false;
					position++;
				break;
			}
		}
	}
}
