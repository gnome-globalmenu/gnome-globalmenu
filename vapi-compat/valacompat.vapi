using GLib;
[CCode (cprefix = "G", lower_case_cprefix = "g_", cheader_filename = "glib.h")]
namespace GLibCompat {
	[CCode (ref_function = "g_object_ref", unref_function = "g_object_unref", marshaller_type_name = "OBJECT", get_value_function = "g_value_get_object", set_value_function = "g_value_set_object", param_spec_function = "g_param_spec_object", cheader_filename = "glib-object.h")]
	public class Object {
		[CCode (cname = "g_object_notify")]
		public void notify_property(string property);
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
