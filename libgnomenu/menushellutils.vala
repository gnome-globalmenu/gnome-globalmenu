namespace Gnomenu {
	[CCode (cname="gtk_menu_shell_get_item")]
	public extern weak Gtk.MenuItem gtk_menu_shell_get_item(Gtk.MenuShell * menu_shell, int position);
	/*
	[CCode (cname="gtk_menu_shell_has_item")]
	public extern bool gtk_menu_shell_has_item(Gtk.MenuShell * menu_shell, int position);
	*/
	[CCode (cname="gtk_menu_shell_truncate")]
	public extern void gtk_menu_shell_truncate(Gtk.MenuShell * menushell, int length);
	[CCode (cname="gtk_menu_shell_length")]
	public extern int gtk_menu_shell_length(Gtk.MenuShell * menu_shell);
}
