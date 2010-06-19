public class Gnomenu.Settings : Object {
	public Gdk.Window window {get; private set;}

	private Gdk.Atom atom = Gdk.Atom.intern("_NET_GLOBALMENU_SETTINGS", false);

	public KeyFile keyfile = new KeyFile();

	public virtual bool show_local_menu { get; set; default = true; }
	public virtual bool show_menu_icons { get; set; default = true; }
	public virtual int changed_notify_timeout { get; set; default = 500; }
	
	public static const string[] KEYS = {
		"show-local-menu",
		"show-menu-icons",
		"changed-notify-timeout"
	};

	public virtual void attach_to_window(Gdk.Window? window) {
		if(this.window != null) {
			window.remove_filter(this.event_filter);
		}

		if(window == null) {
			return;
		}

		this.window = window;
		this.window.add_filter(this.event_filter);
		var events = this.window.get_events();
		this.window.set_events(events | Gdk.EventMask.PROPERTY_CHANGE_MASK);
		pull();
	}

	~Settings() {
		attach_to_window(null);
	}

	[CCode (instance_pos = -1)]
	private Gdk.FilterReturn event_filter(Gdk.XEvent xevent, Gdk.Event event) {
		/* This weird extra level of calling is to avoid a type cast in Vala
		 * which will cause the loss of delegate target. */
		void * pointer = &xevent;
		return real_event_filter((X.Event*)pointer, event);
	}
	private bool atom_equal(Gdk.Atom a1, Gdk.Atom a2) {
		return (int)a1 == (int)a2;
	}
	[CCode (instance_pos = -1)]
	private Gdk.FilterReturn real_event_filter(X.Event* xevent, Gdk.Event event) {
		switch(xevent->type) {
			case X.EventType.PropertyNotify:
			Gdk.Atom atom_in = Gdk.x11_xatom_to_atom(xevent->xproperty.atom);
			if(atom_equal(atom, atom_in)) {
				pull();
			}
			break;
		}
		return Gdk.FilterReturn.CONTINUE;
	}

	private static bool is_tristate(bool boolean) {
		return boolean != true && boolean != false;
	}

	private bool load_boolean(string key) {
		try {
			return keyfile.get_boolean("GlobalMenu:Client", key);
		} catch{
			return (bool) 30;
		}
	}

	private int load_int(string key) {
		try {
		return keyfile.get_integer("GlobalMenu:Client", key);
		} catch{
			return -1;
		}
	}
	private string? load_string(string key) {
		try {
		return keyfile.get_string("GlobalMenu:Client", key);
		} catch{
			return null;
		}
	}

	private void save_boolean(string key, bool value) {
		try {
		if(!is_tristate(value)) {
			keyfile.set_boolean("GlobalMenu:Client", key, value);
		} else {
			keyfile.remove_key("GlobalMenu:Client", key);
		}
		} catch{}
	}
	private void save_string(string key, string? value) {
		try {
		if(value != null) {
			keyfile.set_string("GlobalMenu:Client", key, value);
		} else {
			keyfile.remove_key("GlobalMenu:Client", key);
		}
		} catch{}
	}
	private void save_int(string key, int value) {
		keyfile.set_integer("GlobalMenu:Client", key, value);
	}

	private void save_property(string key) {
		weak ObjectClass klass = (ObjectClass) (this.get_class());
		weak ParamSpec pspec = klass.find_property(key);
		Value value = Value(pspec.value_type);
		this.get_property(key, ref value);
		if(pspec.value_type == typeof(bool)) {
			save_boolean(key, value.get_boolean());
		} else
		if(pspec.value_type == typeof(string)) {
			save_string(key, value.get_string());
		} else
		if(pspec.value_type == typeof(int)) {
			save_int(key, value.get_int());
		} else {
			stdout.printf("unsupported value type `%s'.\n", 
				pspec.value_type.name());
			return;
		}
	}

	private void load_property(string key) {
		weak ObjectClass klass = (ObjectClass) (this.get_class());
		weak ParamSpec pspec = klass.find_property(key);
		Value value = Value(pspec.value_type);
		if(pspec.value_type == typeof(bool)) {
			value.set_boolean(load_boolean(key));
		} else
		if(pspec.value_type == typeof(string)) {
			value.set_string(load_string(key));
		} else
		if(pspec.value_type == typeof(int)) {
			value.set_int(load_int(key));
		} else {
			stdout.printf("unsupported value type `%s'.\n", 
				pspec.value_type.name());
			return;
		}
		this.set_property(key, value);
	}

	public string to_string() {
		foreach(var key in KEYS) {
			save_property(key);
		}
		return keyfile.to_data(null);
	}

	public void pull() {
		var data = get_by_atom(atom);
		if(data == null) return;
		keyfile.load_from_data(data, data.length, KeyFileFlags.NONE);
		foreach(var key in KEYS) {
			load_property(key);
		}
	}

	public void push() {
		var str = this.to_string();
		set_by_atom(atom, str);
	}

	public string? get_by_atom(Gdk.Atom atom) {
		string context = null;
		Gdk.Atom actual_type;
		Gdk.Atom type = Gdk.Atom.intern("STRING", false);
		int actual_format;
		int actual_length;
		Gdk.property_get(window,
			atom,
			type,
			0, (ulong) long.MAX, false, 
			out actual_type, 
			out actual_format, 
			out actual_length, 
			out context);
		return context;
	}

	public void set_by_atom(Gdk.Atom atom, string? value) {
		if(value != null) {
			Gdk.Atom type = Gdk.Atom.intern("STRING", false);
			Gdk.property_change(window,
				atom, type,
				8,
				Gdk.PropMode.REPLACE,
				value, 
				(int) value.size() + 1
			);
		} else {
			Gdk.property_delete(window, atom);
		}
	}
}
