#ifndef _MENU_H_
#define _MENU_H_
void gnomenu_disable();
gboolean gnomenu_init();
void gnomenu_wrap_widget(GtkWidget * widget);
void gnomenu_bind_menu(GdkWindow * window, GtkWidget * menu);
void gnomenu_unbind_menu(GdkWindow * window, GtkWidget * menu);
#endif
