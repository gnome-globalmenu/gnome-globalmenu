[CCode (cprefix = "GConf", lower_case_cprefix = "gconf_")]
namespace GConfCompat {
	[Compact]
	[CCode (copy_function = "gconf_schema_copy", cheader_filename = "gconf/gconf.h")]
	public class Schema : GConf.Schema {
		public GConf.ValueType get_type ();
	}
}
