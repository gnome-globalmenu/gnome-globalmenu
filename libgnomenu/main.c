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
gboolean gnomenu_compatible = FALSE;
void gtk_module_init(int * argc, char **argv[]){
	GnomenuQuirkMask mask = gnomenu_get_default_quirk();
	LOG("work as a gtk_module");

	if(GNOMENU_HAS_QUIRK(mask, IGNORE)){
		gnomenu_compatible = FALSE;
	} else {
		gnomenu_compatible = TRUE;
	}
}
