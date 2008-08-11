#ifndef _MENU_H_
#define _MENU_H_
gboolean gnomenu_init();
GQuark gnomenu_wrap_widget(GtkWidget * widget);
void gnomenu_bind_menu(GdkWindow * window, GQuark object);
void gnomenu_unbind_menu(GdkWindow * window, GQuark object);
void gnomenu_unwrap_widget(GQuark object);
GtkWidget * gnomenu_find_widget(GQuark object);
#endif
