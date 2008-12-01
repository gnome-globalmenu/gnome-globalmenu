#include <gtk/gtk.h>
#include "dyn-patch-helper.h"

void dyn_patch_menu_item() {
	GObjectClass * klass =  g_type_class_ref(GTK_TYPE_MENU_ITEM);

}
