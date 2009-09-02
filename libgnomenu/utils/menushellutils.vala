namespace Gnomenu {
	[CCode (cname="gtk_menu_shell_get_item")]
	public extern weak Gtk.MenuItem gtk_menu_shell_get_item(Gtk.MenuShell * menu_shell, int position);
	[CCode (cname="gtk_menu_shell_truncate")]
	public extern void gtk_menu_shell_truncate(Gtk.MenuShell * menushell, int length);
	[CCode (cname="gtk_menu_shell_length_without_truncated")]
	public extern int gtk_menu_shell_length_without_truncated(Gtk.MenuShell * menu_shell);
	[CCode (cname="gtk_menu_shell_get_item_position")]
	public extern int gtk_menu_shell_get_item_position(Gtk.MenuShell * menu_shell, Gtk.MenuItem * item);
}
