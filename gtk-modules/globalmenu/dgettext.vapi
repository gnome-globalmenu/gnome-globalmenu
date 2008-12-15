namespace GLib {
	[CCode (cname="g_dgettext", cheader_filename="glib.h,glib/gi18n-lib.h")]
	public weak string dgettext(string domain, string msgid);
}
