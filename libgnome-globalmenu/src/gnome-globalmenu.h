#ifndef __GTK_GLOBAL_MENU_BAR_H__
#define __GTK_GLOBAL_MENU_BAR_H__


#include <gdk/gdk.h>
#include <gtk/gtkmenushell.h>


G_BEGIN_DECLS


#define	GTK_TYPE_GLOBAL_MENU_BAR               (gtk_global_menu_bar_get_type ())
#define GTK_GLOBAL_MENU_BAR(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_GLOBAL_MENU_BAR, GtkGlobalMenuBar))
#define GTK_GLOBAL_MENU_BAR_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_GLOBAL_MENU_BAR, GtkGlobalMenuBarClass))
#define GTK_IS_GLOBAL_MENU_BAR(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_GLOBAL_MENU_BAR))
#define GTK_IS_GLOBAL_MENU_BAR_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_GLOBAL_MENU_BAR))
#define GTK_GLOBAL_MENU_BAR_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_GLOBAL_MENU_BAR, GtkGlobalMenuBarClass))

typedef struct _GtkGlobalMenuBar       GtkGlobalMenuBar;
typedef struct _GtkGlobalMenuBarClass  GtkGlobalMenuBarClass;

struct _GtkGlobalMenuBar
{
  GtkMenuBar menu_bar;
};

struct _GtkGlobalMenuBarClass
{
  GtkMenuBarClass parent_class;

  /* Padding for future expansion */
  void (*_gtk_reserved1) (void);
  void (*_gtk_reserved2) (void);
  void (*_gtk_reserved3) (void);
  void (*_gtk_reserved4) (void);
};


GType      gtk_global_menu_bar_get_type        (void) G_GNUC_CONST;
GtkWidget* gtk_global_menu_bar_new             (void);

GtkPackDirection gtk_global_menu_bar_get_pack_direction (GtkMenuBar       *menubar);
void             gtk_global_menu_bar_set_pack_direction (GtkMenuBar       *menubar,
						  GtkPackDirection  pack_dir);
GtkPackDirection gtk_global_menu_bar_get_child_pack_direction (GtkMenuBar       *menubar);
void             gtk_global_menu_bar_set_child_pack_direction (GtkMenuBar       *menubar,
							GtkPackDirection  child_pack_dir);

#ifndef GTK_DISABLE_DEPRECATED
#define gtk_global_menu_bar_append(menu,child)	    gtk_menu_shell_append  ((GtkMenuShell *)(menu),(child))
#define gtk_global_menu_bar_prepend(menu,child)    gtk_menu_shell_prepend ((GtkMenuShell *)(menu),(child))
#define gtk_global_menu_bar_insert(menu,child,pos) gtk_menu_shell_insert ((GtkMenuShell *)(menu),(child),(pos))
#endif /* GTK_DISABLE_DEPRECATED */

/* Private functions */
void _gtk_global_menu_bar_cycle_focus (GtkGlobalMenuBar       *menubar,
				GtkDirectionType  dir);


G_END_DECLS


#endif /* __GTK_GLOBAL_MENU_BAR_H__ */
