using GLib;
[CCode (cprefix = "G", lower_case_cprefix = "g_", cheader_filename = "glib.h")]
namespace GLibCompat {
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

	public class ParamSpecBoolean : ParamSpec {
		[CCode (cname = "g_param_spec_boolean")]
		public ParamSpecBoolean (string name, string nick, string blurb, bool defaultvalue, ParamFlags flags);
	}
	[SimpleType]
	[CCode (cname = "gconstpointer", cheader_filename = "glib.h", type_id = "G_TYPE_POINTER", marshaller_type_name = "POINTER", get_value_function = "g_value_get_pointer", set_value_function = "g_value_set_pointer", default_value = "NULL")]
	public struct constpointer {
		[CCode (cname ="g_dataset_destroy")]
		public void destroy ();
		[CCode (cname ="g_dataset_id_get_data")]
		public void* id_get_data (Quark key_id);
		[CCode (cname ="g_dataset_id_set_data")]
		public void id_set_data (Quark key_id, void* data);
		[CCode (cname ="g_dataset_id_set_data_full")]
		public void id_set_data_full (Quark key_id, void* data, DestroyNotify? destroy_func);
		[CCode (cname ="g_dataset_id_remove_data")]
		public void id_remove_data (Quark key_id);
		[CCode (cname ="g_dataset_id_remove_no_notify")]
		public void* id_remove_no_notify (Quark key_id);
		[CCode (cname ="g_dataset_foreach")]
		public void @foreach (DataForeachFunc func);
		[CCode (cname ="g_dataset_get_data")]
		public void* get_data (string key);
		[CCode (cname ="g_dataset_set_data_full")]
		public void set_data_full (string key, void* data, DestroyNotify? destry_func);
		[CCode (cname ="g_dataset_remove_no_notify")]
		public void* remove_no_notify (string key);
		[CCode (cname ="g_dataset_set_data")]
		public void set_data (string key, void* data);
		[CCode (cname ="g_dataset_remove_data")]
		public void remove_data (string key);
	}
}
