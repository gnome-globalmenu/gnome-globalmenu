public bool gdk_property_get (Gdk.Window window, 
		Gdk.Atom property, 
		Gdk.Atom type, 
		ulong offset, ulong length, 
		bool pdelete, 
		out Gdk.Atom actual_property_type, 
		out int actual_format, 
		out int actual_length, 
		out string data);

public bool gdk_property_change (Gdk.Window window, 
		Gdk.Atom property, 
		Gdk.Atom type, 
		int format,
		Gdk.PropMode mode,
		string data,
		int bytes_size);
public Gdk.Window gdk_window_foreign_new(ulong native);
