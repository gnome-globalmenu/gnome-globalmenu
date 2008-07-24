#include <config.h>
#include <gtk/gtk.h>
#include <libgnomenu/globalmenu.h>
#include <glade/glade.h>
//#include <libgtkhotkey/gtkhotkey.h>
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#undef WNCK_I_KNOW_THIS_IS_UNSTABLE

static void 
	_s_screen_active_window_changed	(WnckScreen * screen, WnckWindow * previous, GnomenuGlobalMenu * global_menu){
	WnckWindow * active = wnck_screen_get_active_window(screen);
	if (!active) return;
	if( wnck_window_get_pid(active) == getpid()){
		return;
	}
	if(global_menu->active_key !=wnck_window_get_xid(active)) {
		gnomenu_global_menu_switch(global_menu, wnck_window_get_xid(active));
	}
}
/*
static void
_s_hkf10_activated(GtkHotkeyInfo * hkinfo, guint event_time, GtkWidget * window){
	g_message("f10");
	gtk_window_present(window);
}
*/
int main(int argc, char * argv[]){
	gtk_init(&argc, &argv);
	g_log_set_always_fatal(G_LOG_LEVEL_CRITICAL);
	GladeXML * xml;
	GtkWidget * window;
	GnomenuGlobalMenu * globalmenu;
	/*
	GtkHotkeyInfo * hkf10 = gtk_hotkey_info_new(
			"GlobalMenu", "MenuKey", "F9", NULL);
	gtk_hotkey_info_bind(hkf10, NULL);
	*/
	gnomenu_global_menu_get_type();
	xml = glade_xml_new("GnomenuServerWindow.glade", NULL, NULL);
	g_assert(xml);
	window = glade_xml_get_widget(xml, "GnomenuServerWindow");
	globalmenu = glade_xml_get_widget(xml, "globalmenu");
//	globalmenu->auto_switch = TRUE;
	glade_xml_signal_autoconnect(xml);

	g_signal_connect(wnck_screen_get_default(),
			"active-window-changed", G_CALLBACK(_s_screen_active_window_changed), globalmenu);
//	g_signal_connect(hkf10, "activated", _s_hkf10_activated, window);
	gtk_window_set_keep_above(window, TRUE);
	gtk_widget_show(window);
	gtk_main();
	g_signal_handlers_disconnect_by_func(wnck_screen_get_default(),
			_s_screen_active_window_changed, globalmenu);
	g_object_unref(xml);
	return 0;
}
/*
vim:ts=4:sw=4
*/
