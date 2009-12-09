
/**
 * Parser converts xml to widgets.
 *
 */
public class Gnomenu.Parser {
	public static void parse(Shell shell, string description) throws GLib.Error {
		var parser = new Parser(shell);
		var timer = new Timer();
		MarkupParseContext context = 
			new MarkupParseContext(parser_functions, 0, parser, null);
		context.parse(description, -1);
		debug("Parser consumed: %lf for %lu bytes", timer.elapsed(null),
				description.size());
	}

	private class State {
		public Shell shell;
		/* counting from zero, the position of current item.
		 * when the menu tag is closed,
		 * this number is also used to truncated the shell.
		 * */
		private int _position = 0;
		public int position {
			get {
				return _position;
			}
		}
		public void advance() {
			_position++;
		}
		public Item get_item() {
			return shell.get_item(position);
		}

		/* item_has_sub_shell is used to defer the removal of the
		 * sub menu shell to the close tag of the item, when
		 * we are 100% sure whether there is a sub shell or not.
		 *
		 * because we can't blindly set item.has_sub_shell = false at the
		 * open tag handler. Doing so will get the popup submenus 
		 * crazy if they were already popped up.
		 * */
		public bool item_has_sub_shell = false;
		public State(Shell shell) {
			this.shell = shell;
		}
	}

	Parser (Shell shell) {
		stack = new Queue<State>();
		State bootstrap = new State(shell);
		is_bootstrapping = true;
		stack.push_tail(bootstrap);
	}

	~Parser() {
		State s = null;
		while((s = stack.pop_tail()) != null) {
			continue;
		}
	}
	static const MarkupParser parser_functions = {
		start_element,
		end_element,
		null, null, null
	};

	Queue<State> stack;
	bool is_bootstrapping;
	weak State state  {
		get {
			return stack.peek_tail();
		}
	}

	private void start_element (MarkupParseContext context, 
			string element_name, 
			string[] attribute_names, 
			string[] attribute_values) throws MarkupError {
		switch(element_name){
			case "menu":
				if(!is_bootstrapping) {
					/*if this is not the root <menu> entry
					 * aka, we are at
					 * <menu><item><MENU>*/
					var item = state.get_item();
					state.item_has_sub_shell = true;
					/* nested the menu, change state*/
					stack.push_tail(new State(item.sub_shell));
				}
			break;
			case "item":
				is_bootstrapping = false;
				/*NOTE: after the first time we has(position) == false,
				 * it should be false forever)*/
				var item = state.get_item();
				setup_item(item, attribute_names, attribute_values);
				state.item_has_sub_shell = false;
			break;
			default:
				throw new MarkupError.UNKNOWN_ELEMENT("unkown element");
		}
	}
	private void setup_item(Item item, 
			string[] attr_names, 
			string[] attr_vals) throws GLib.Error {
		weak string label = null;
		weak string icon = null;
		weak string type = null;
		weak string state = null;
		weak string font = null;
		weak string id = null;
		weak string accel = null;
		bool sensitive = true;
		bool visible = true;
		bool underline = true;
		bool client_side = false;

		collect_attributes("item", attr_names, attr_vals,
				Markup.CollectType.STRING | Markup.CollectType.OPTIONAL,
				"label", &label, 
				Markup.CollectType.STRING | Markup.CollectType.OPTIONAL,
				"type", &type, 
				Markup.CollectType.STRING | Markup.CollectType.OPTIONAL,
				"state", &state, 
				Markup.CollectType.STRING | Markup.CollectType.OPTIONAL,
				"font", &font, 
				Markup.CollectType.STRING | Markup.CollectType.OPTIONAL,
				"id", &id, 
				Markup.CollectType.STRING | Markup.CollectType.OPTIONAL,
				"icon", &icon, 
				Markup.CollectType.STRING | Markup.CollectType.OPTIONAL,
				"accel", &accel, 
				Markup.CollectType.TRISTATE,
				"visible", &visible, 
				Markup.CollectType.TRISTATE,
				"underline", &underline, 
				Markup.CollectType.TRISTATE,
				"sensitive", &sensitive, 
				Markup.CollectType.TRISTATE,
				"client-side", &client_side
				);

		if(visible != false)
			visible = true;
		
		if(sensitive != false)
			sensitive = true;

		if(underline != false)
			underline = true;

		if(client_side != true) {
			client_side = false;
		}
		item.item_id = id;
		item.item_visible = visible;
		item.item_use_underline = underline;
		item.item_sensitive = sensitive;
		item.item_type = Item.type_from_string(type);
		item.item_accel_text = accel;
		item.item_label = label;
		item.item_icon= icon;
		item.item_state = Item.state_from_string(state);
		item.item_font = font;
		item.client_side_sub_shell = client_side;
	}
	private void end_element (MarkupParseContext context, 
			string element_name) throws MarkupError {
		switch(element_name) {
			case "menu":
				/* truncate the shell, */
				state.shell.length = state.position;
				/* then move back to the parent shell.
				 * notice the bootstrap state is also
				 * popped out.
				 **/
				stack.pop_tail();
				break;
			case "item":
				if(!state.item_has_sub_shell) {
					state.get_item().has_sub_shell = false;
				} else {
					state.get_item().has_sub_shell = true;
				}
				state.advance();
			break;
		}
	}
}
