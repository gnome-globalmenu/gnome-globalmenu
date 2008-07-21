/*
 * GnomenuGlobalMenu
 * */
#ifndef __GNOMENU_GLOBAL_MENU_H__
#define __GNOMENU_GLOBAL_MENU_H__
/**
 * SECTION: globalmenu
 *
 * A view of the active global menu bar.
 */

#include <gdk/gdk.h>
#include "menubar.h"

G_BEGIN_DECLS


#define	GNOMENU_TYPE_GLOBAL_MENU               (gnomenu_global_menu_get_type ())
#define GNOMENU_GLOBAL_MENU(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOMENU_TYPE_GLOBAL_MENU, GnomenuGlobalMenu))
#define GNOMENU_GLOBAL_MENU_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), GNOMENU_TYPE_GLOBAL_MENU, GnomenuGlobalMenuClass))
#define GNOMENU_IS_GLOBAL_MENU(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOMENU_TYPE_GLOBAL_MENU))
#define GNOMENU_IS_GLOBAL_MENU_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), GNOMENU_TYPE_GLOBAL_MENU))
#define GNOMENU_GLOBAL_MENU_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), GNOMENU_TYPE_GLOBAL_MENU, GnomenuGlobalMenuClass))

typedef struct _GnomenuGlobalMenu       GnomenuGlobalMenu;
typedef struct _GnomenuGlobalMenuClass  GnomenuGlobalMenuClass;

struct _GnomenuGlobalMenu
{
	GtkContainer parent;
	GHashTable * cache;
	gpointer active_key;
	GnomenuMenuBar * active_menu_bar;
};

/**
 * GnomenuGlobalMenuClass:
 *
 */
struct _GnomenuGlobalMenuClass
{
  GtkContainerClass parent_class;
};


GtkWidget * gnomenu_global_menu_new             (void);
void gnomenu_global_menu_switch (GnomenuGlobalMenu * self, gpointer key);
GType gnomenu_global_menu_get_type (void);

G_END_DECLS

#endif /* __GNOMENU_GLOBAL_MENU_H__ */
