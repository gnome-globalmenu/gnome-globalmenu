public int main(string[] args) {
	Gtk.init(ref args);
	Gnomenu.Settings settings = new Gnomenu.Settings(Gdk.Screen.get_default());
	if(args.length < 3) {
		message("Usage: %s key value", args[0]);
		return 0;
	}
	
	weak ObjectClass klass = (ObjectClass) settings.get_type().class_peek();
	weak ParamSpec pspec = klass.find_property(args[1]);
	if(pspec == null) {
		message("property %s unknown", args[1]);
		weak ParamSpec[] pspecs = klass.list_properties();
		foreach(weak ParamSpec ps in pspecs) {
			message("%s : %s", ps.name, ps.value_type.name());
		}
		return 0;
	}
	Value value = Value(pspec.value_type);
	if(pspec.value_type == typeof(bool)) {
		value.set_boolean(args[2].to_bool());
	} else
	if(pspec.value_type == typeof(int)) {
		value.set_int(args[2].to_int());
	} else {
		message("unsupported type %s", pspec.value_type.name());
		return 0;
	}
	settings.set_property(args[1], value);
	settings.push();
	Gdk.flush();

	return 0;
}
