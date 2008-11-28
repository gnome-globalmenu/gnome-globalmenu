using Gtk;

namespace Gnomenu {
	public class Parser {
		Parser (MenuShell shell) {
			position = 0; 
			inside_item = false;
			this.shell = shell;
			MarkupParser parser_funcs = {
					start_element,
					end_element,
					null, null, null
			};
			Memory.copy(&this.functions, &parser_funcs, sizeof(MarkupParser));
		}
		public static void parse(MenuShell shell, string description) throws GLib.Error {
			var parser = new Parser(shell);
			MarkupParseContext context = 
				new MarkupParseContext(parser.functions, 0, parser, null);
			context.parse(description, -1);
		}


		MarkupParser functions;
		MenuShell shell;
		int position;
		bool inside_item;
		Parser child_parser; /*to hold the ref count*/
		private void start_element (MarkupParseContext context, string element_name, string[] attribute_names, string[] attribute_values) throws MarkupError {
			switch(element_name){
				case "menu":
					if(inside_item == false) {
						/*Ignore it, allowing 
						 * <menu> <item/><item/><item/> </menu>
						 */
					} else {
						weak MenuItem item = shell.get(position);
						if(item.submenu == null) {
							item.submenu = new Menu();
						}	
						child_parser = new Parser(item.submenu);
						g_markup_parse_context_push(context, functions, child_parser);
					}
				break;
				case "item":
					/*NOTE: after the first time we has(position) == false,
					 * it should be false forever)*/
					if(shell.has(position)) {
						var item = shell.get(position);
						/* do stuff, setup_item(attributes...?)*/
					} else {
						var item = new MenuItem();
						/*setup_item(...) */
						shell.append(item);
					}
					inside_item = true;
				break;
				default:
					throw new MarkupError.UNKNOWN_ELEMENT("unkown element");
			}
		}
		private void end_element (MarkupParseContext context, string element_name) throws MarkupError {
			switch(element_name) {
				case "menu":
					if(inside_item) {
						/* stop the child parser */
						g_markup_parse_context_pop(context);
						child_parser = null;
					}
					break;
				case "item":
					position++;
					inside_item = false;
				break;
			}
		}
	}
}
