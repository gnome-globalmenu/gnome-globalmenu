
public static class Module {
	[CCode (cname="g_module_check_init")]
	public static string? g_module_load(Module module) {
		string app_name = Environment.get_prgname();
		if(app_name != "gnome-panel") 
			return "GnomenuPanel only works with gnome-panel.";
		return null;
	}
}
