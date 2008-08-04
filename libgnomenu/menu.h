#ifndef _MENU_H_
#define _MENU_H_
gboolean gnomenu_init();
GQuark gnomenu_create(const gchar * hint);
gboolean gnomenu_set_property(GQuark item, gchar * property, gchar * value);
gboolean gnomenu_get_property(GQuark item, gchar * property, gchar ** value);
gboolean gnomenu_insert_child(GQuark menu, GQuark item, gint pos);
gboolean gnomenu_remove_item(GQuark menu, GQuark item);
gboolean gnomenu_destroy(GQuark object);
#endif
