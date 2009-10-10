
public static class Plugin {
	public static bool disabled = false;
	[CCode (cname="g_module_check_init")]
	public static string? g_module_load(Module module) {
		string app_name = Environment.get_prgname();
		if(app_name != "gnome-panel") {
			disabled = true;
			return "GnomenuPanel only works with gnome-panel.";
		}
		return null;
	}
	private static TypeClass klass;
	private static Type panel_menu_bar_type;
	[CCode (cname="gtk_module_init")]
	public static void gtk_module_init([CCode (array_length_pos = 0.9)] ref string[] args) {
		module = Module.open(null, 0);
		panel_menu_bar_type = panel_menu_bar_get_type();
		klass = panel_menu_bar_type.class_ref();
		Signal.add_emission_hook(Signal.lookup("hierarchy-changed", panel_menu_bar_type),
			0, hierarchy_changed, null);
		
	}

	private static bool hierarchy_changed(SignalInvocationHint ihint,
		[CCode (array_length_pos = 1.9)]
		Value[] param_values) {
		Object object = param_values[0].get_object();
		if(!object.get_type().is_a(panel_menu_bar_type)) return true;
		message("hierarch_changed! %s", object.get_type().name());
		hack(object as Gtk.MenuBar);
		return true;
	}

	private static void hack(Gtk.MenuBar widget) {
		if((bool)widget.get_data("hacked") == true) {
			return;
		}
		widget.set_data("hacked", (void*) true);
		message("hacked");
		var adapter = new Gnomenu.GlobalMenuAdapter(widget);

	}
	private static Module module = null;

	static delegate Type GTypeFunction();

	private static Type panel_menu_bar_get_type() {
		void * pointer = null;
		module.symbol("panel_menu_bar_get_type", out pointer);
		GTypeFunction function = (GTypeFunction)pointer;
		return function();
	}
}
