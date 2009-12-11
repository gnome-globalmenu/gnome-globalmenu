private string[] pairs;
private bool show = false;
private ObjectClass klass = null;
private Gnomenu.Settings settings = null;
private ulong xid = 0;

private const OptionEntry[] options = {
	{"show", 's', 0, OptionArg.NONE, ref show, N_("Show current settings"), null },
	{"window", 'w', 0, OptionArg.INT, ref xid, N_("Access local settings of the window(XWin ID)"), null },
	{"", 0, 0, OptionArg.STRING_ARRAY, ref pairs, N_("key/setting pairs"), "[ key settings ] ..."},
	{null}
};
public int main(string[] args) {

	Intl.bindtextdomain(Config.GETTEXT_PACKAGE, Config.LOCALEDIR);
	Intl.bind_textdomain_codeset (Config.GETTEXT_PACKAGE, "UTF-8");
	Intl.textdomain (Config.GETTEXT_PACKAGE);

	klass = (ObjectClass) (typeof(Gnomenu.Settings).class_ref());

	OptionContext context = new OptionContext(" -- Modify Global Menu Settings");
	context.set_description(
N_("""A tool to modify Global Menu settings.""")
);
	context.set_help_enabled (true);
	context.add_group(Gtk.get_option_group(true));
	context.add_main_entries(options, null);

	context.parse(ref args);

	if(xid == 0) {
		settings = Gnomenu.GlobalSettings.get(Gdk.Screen.get_default());
	} else {
		settings = new Gnomenu.LocalSettings(Gdk.Window.foreign_new((Gdk.NativeWindow)xid));
	}

	if(show) {
		list_settings();
		return 0;
	}

	if(pairs == null 
	|| (pairs.length = (int)strv_length(pairs)) == 0
	|| pairs.length % 2 != 0) {
		stdout.printf("%s\n", context.get_help(false, null));
		return 0;
	}

	for(int i = 0; i < pairs.length; i+=2) {
		set_key(pairs[i], pairs[i+1]);
	}

	settings.push();
	Gdk.flush();

	return 0;
}

private void list_settings() {
	foreach(var key in Gnomenu.Settings.KEYS) {
		weak ParamSpec ps = klass.find_property(key);
		Value value = Value(ps.value_type);
		settings.get_property(ps.name, ref value);
		stdout.printf("%s (%s) = %s\n", ps.name, ps.value_type.name(), value.strdup_contents());
	}
}

private void set_key(string key, string v) {
	weak ParamSpec pspec = klass.find_property(key);
	if(pspec == null) {
		stdout.printf("key `%s' is not supported.\n", key);
		return;
	}

	Value value = Value(pspec.value_type);
	if(pspec.value_type == typeof(bool)) {
		value.set_boolean(v.to_bool());
	} else
	if(pspec.value_type == typeof(string)) {
		value.set_string(v);
	} else
	if(pspec.value_type == typeof(int)) {
		value.set_int(v.to_int());
	} else {
		stdout.printf("unsupported value type `%s'.\n", 
			pspec.value_type.name());
		return;
	}
	settings.set_property(key, value);
}
