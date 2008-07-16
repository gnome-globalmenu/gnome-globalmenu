#include <config.h>
#include <gtk/gtk.h>
#include <libgnomenu/globalmenu.h>
#include <glade/glade.h>
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
	gnomenu_global_menu_switch(global_menu, wnck_window_get_xid(active));
}
int main(int argc, char * argv[]){
	gtk_init(&argc, &argv);

	GladeXML * xml;
	GtkWidget * window;
	GnomenuGlobalMenu * globalmenu;
	gnomenu_global_menu_get_type();
	xml = glade_xml_new("GnomenuServerWindow.glade", NULL, NULL);
	window = glade_xml_get_widget(xml, "GnomenuServerWindow");
	globalmenu = glade_xml_get_widget(xml, "globalmenu");
//	globalmenu->auto_switch = TRUE;
	glade_xml_signal_autoconnect(xml);

	g_signal_connect(wnck_screen_get_default(),
			"active-window-changed", G_CALLBACK(_s_screen_active_window_changed), globalmenu);
	gtk_widget_show(window);
	gtk_main();
	g_object_unref(xml);
	return 0;
}
/*
vim:ts=4:sw=4
*/
