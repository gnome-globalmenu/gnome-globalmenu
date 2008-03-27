#include <config.h>
#include <gtk/gtk.h>

#include "quirks.h"
#include "menubar.h"

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_message("<GnomenuMain>::" fmt, ## args)
#else
#define LOG(fmt, args...)
#endif

#define LOG_FUNC_NAME LOG("%s", __func__)

/**
 * gnomenu_menu_bar_type:
 *
 * Zero if GtkMenuBar shall not be replaced by GnomenuMenuBar.
 * Or else the value of gnomenu_menu_bar_get_type();
 * */
GType gnomenu_menu_bar_type = 0;
/**
 * gnomenu_compatible:
 *
 * TRUE if works in compatible mode.
 * FASLE: disable gtk_menu_bar_get_type hack by default
 * */
G_MODULE_EXPORT gboolean gnomenu_compatible = FALSE;
gint gnomenu_version = 4;
const char * g_module_check_init(GModule * module){
	GnomenuQuirkMask mask = gnomenu_get_default_quirk();
	gchar *flags[] = {
					"GTK_MENUBAR_NO_MAC", 		"GTK_MENU_BAR_NO_MAC",
					"GTK_MENUBAR_NO_GLOBALMENU","GTK_MENU_BAR_NO_GLOBALMENU",
					"GTK_MENUBAR_NO_GLOBAL", 	"GTK_MENU_BAR_NO_GLOBAL",
					"GTK_MENUBAR_NO_GNOMENU", 	"GTK_MENU_BAR_NO_GNOMENU", NULL
			};
	int i;
	LOG("libgnomenu is loaded. ");

	if(GNOMENU_HAS_QUIRK(mask, IGNORE)){
		gnomenu_compatible = FALSE;
		LOG("application is ignored by quirk");
		return NULL;
	}
	for(i = 0;flags[i];i++)
		if(getenv(flags[i])){
			gnomenu_compatible = FALSE;
			LOG("application is ignored by env %s flag", flags[i]);
			return NULL;
		}
	gnomenu_compatible = TRUE;
	return NULL;
}
//void gtk_module_init(int * argc, char **argv[]){
	/*Do nothing*/
//}
/*
 * vim: ts=4:sw=4
 * */
