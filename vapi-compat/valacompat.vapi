using GLib;
[CCode (cprefix = "G", lower_case_cprefix = "g_", cheader_filename = "glib.h")]
namespace GLibCompat {
	[Compact]
	[CCode (free_function = "g_string_chunk_free")]
	public class StringChunk {
		public StringChunk (ulong size);
		public weak string insert(string str);
		public weak string insert_const(string str);
		public weak string insert_len(void * buffer, ulong len);
		public void clear();
	}
	[Compact]
	[Immutable]
	[CCode (cname = "char", const_cname = "const char", copy_function = "g_strdup", free_function = "g_free", cheader_filename = "stdlib.h,string.h,glib.h", type_id = "G_TYPE_STRING", marshaller_type_name = "STRING", get_value_function = "g_value_get_string", set_value_function = "g_value_set_string", type_signature = "s")]
	public class String {
		[CCode (cname = "g_strcanon")]
		public weak string canon(string valid_chars, char substitutor);
	}
	[CCode (lower_case_csuffix = "object_class")]
	public class ObjectClass : TypeClass {
		public weak ParamSpec find_property (string property_name);
		public weak ParamSpec[] list_properties ();
		public void install_property  (uint property_id, ParamSpec pspec);
	}
	[CCode (ref_function = "g_object_ref", unref_function = "g_object_unref", marshaller_type_name = "OBJECT", get_value_function = "g_value_get_object", set_value_function = "g_value_set_object", cheader_filename = "glib-object.h")]
	public class Object : TypeInstance {
	
		public void add_toggle_ref(ToggleNotifyFunc notify, void* data);
		public void remove_toggle_ref(ToggleNotifyFunc notify, void* data);
		public static delegate void ToggleNotifyFunc (void* data, GLib.Object object, bool is_last);
	}

	public class ParamSpecBoolean : ParamSpec {
		[CCode (cname = "g_param_spec_boolean")]
		public ParamSpecBoolean (string name, string nick, string blurb, bool defaultvalue, ParamFlags flags);
	}
}
