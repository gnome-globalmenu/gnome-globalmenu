#include <config.h>
#include <gtk/gtk.h>
#include <gnome-desktop-2.0/libgnome/gnome-desktop-item.h>
#define GMENU_I_KNOW_THIS_IS_UNSTABLE
#include <gnome-menus/gmenu-tree.h>
GtkWidget * create_menu_icon(gchar * icon_name){
	GdkPixbuf * pixbuf;
	GError * error = NULL;
	gint width = 0, height = 0;
	gtk_icon_size_lookup(GTK_ICON_SIZE_MENU, &width, &height);

	pixbuf = gdk_pixbuf_new_from_file_at_size(icon_name, width, height, &error);
	if(error) {
		g_error_free(error);
	}
	if(!pixbuf) return gtk_image_new_from_icon_name(icon_name, GTK_ICON_SIZE_MENU);
	return gtk_image_new_from_pixbuf(pixbuf);
}
static void
activate_app_def (GtkWidget      *menuitem,
		  GMenuTreeEntry *entry)
{
	const char       *path;
	GError * error = NULL;
	GnomeDesktopItem * item;
	path = gmenu_tree_entry_get_desktop_file_path (entry);
	item  = gnome_desktop_item_new_from_file(path, 0, &error);
	if(item) {
		gnome_desktop_item_launch(item, NULL, 0, &error);
		gnome_desktop_item_unref(item);
	}
}
static void
create_menuitem (GtkWidget          *menu,
		 GMenuTreeEntry     *entry,
		 GMenuTreeDirectory *alias_directory)
{
	GtkWidget  *menuitem;
	GtkWidget * menuicon;
	gchar * menu_label = 	 gmenu_tree_entry_get_name(entry);
	gchar * menu_icon_name = gmenu_tree_entry_get_icon(entry);
	
	menuitem = gtk_image_menu_item_new_with_mnemonic (menu_label);
	menuicon = create_menu_icon(menu_icon_name);
	gtk_image_menu_item_set_image(menuitem, menuicon);

	g_message("new app menu item %s: icon=%s", menu_label, menu_icon_name);
	g_object_set_data_full (G_OBJECT (menuitem),
				"panel-menu-tree-entry",
				gmenu_tree_item_ref (entry),
				(GDestroyNotify) gmenu_tree_item_unref);

	if (alias_directory)
		//FIXME: we should probably use this data when we do dnd or
		//context menu for this menu item
		g_object_set_data_full (G_OBJECT (menuitem),
					"panel-menu-tree-alias-directory",
					gmenu_tree_item_ref (alias_directory),
					(GDestroyNotify) gmenu_tree_item_unref);

/*	panel_load_menu_image_deferred (menuitem,
					panel_menu_icon_get_size (),
					NULL, NULL,
					alias_directory ? gmenu_tree_directory_get_icon (alias_directory) :
							  gmenu_tree_entry_get_icon (entry),
					NULL);
*/

/*	setup_menuitem (menuitem,
			panel_menu_icon_get_size (),
			NULL,
			alias_directory ? gmenu_tree_directory_get_name (alias_directory) :
					  gmenu_tree_entry_get_name (entry));
*/
/*	if ((alias_directory &&
	     gmenu_tree_directory_get_comment (alias_directory)) ||
	    (!alias_directory &&
	     gmenu_tree_entry_get_comment (entry)))
		panel_util_set_tooltip_text (menuitem,
					     alias_directory ?
						gmenu_tree_directory_get_comment (alias_directory) :
						gmenu_tree_entry_get_comment (entry));
*/
/*	g_signal_connect_after (menuitem, "button_press_event",
				G_CALLBACK (menuitem_button_press_event), NULL);
*/
/*	if (!panel_lockdown_get_locked_down ()) {
		static GtkTargetEntry menu_item_targets[] = {
			{ "text/uri-list", 0, 0 }
		};

		gtk_drag_source_set (menuitem,
				     GDK_BUTTON1_MASK | GDK_BUTTON2_MASK,
				     menu_item_targets, 1,
				     GDK_ACTION_COPY);

		if (gmenu_tree_entry_get_icon (entry) != NULL) {
			const char *icon;
			char       *icon_no_ext;

			icon = gmenu_tree_entry_get_icon (entry);
			if (!g_path_is_absolute (icon)) {
				icon_no_ext = panel_util_icon_remove_extension (icon);
				gtk_drag_source_set_icon_name (menuitem,
							       icon_no_ext);
				g_free (icon_no_ext);
			}
		}

		g_signal_connect (G_OBJECT (menuitem), "drag_begin",
				  G_CALLBACK (drag_begin_menu_cb), NULL);
		g_signal_connect (menuitem, "drag_data_get",
				  G_CALLBACK (drag_data_get_menu_cb), entry);
		g_signal_connect (menuitem, "drag_end",
				  G_CALLBACK (drag_end_menu_cb), NULL);
	}
*/
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);

	g_signal_connect (menuitem, "activate",
			  G_CALLBACK (activate_app_def), entry);

	gtk_widget_show (menuitem);
}


static GtkWidget *
create_submenu_entry (GtkWidget          *menu,
		      GMenuTreeDirectory *directory)
{
	GtkWidget *menuitem;
	GtkWidget *menuicon;

	menuitem = gtk_image_menu_item_new ();
/*
	panel_load_menu_image_deferred (menuitem,
					panel_menu_icon_get_size (),
					NULL, NULL,
					gmenu_tree_directory_get_icon (directory),
					PANEL_ICON_FOLDER);

	setup_menuitem (menuitem,
			panel_menu_icon_get_size (),
			NULL,
			gmenu_tree_directory_get_name (directory));
*/
	gchar * menu_label = 	 gmenu_tree_directory_get_name(directory);
	gchar * menu_icon_name = gmenu_tree_directory_get_icon(directory);

	menuitem = gtk_image_menu_item_new_with_mnemonic (menu_label);
	menuicon = create_menu_icon(menu_icon_name);
	if(menuicon) gtk_image_menu_item_set_image(menuitem, menuicon);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);

	gtk_widget_show (menuitem);

	return menuitem;
}

static void
create_submenu (GtkWidget          *menu,
		GMenuTreeDirectory *directory,
		GMenuTreeDirectory *alias_directory)
{
	GtkWidget *menuitem;
	GtkWidget *submenu;

	if (alias_directory)
		menuitem = create_submenu_entry (menu, alias_directory);
	else
		menuitem = create_submenu_entry (menu, directory);
	
	submenu = gtk_menu_new();
	app_menu_setup_from_directory (submenu, directory);

	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), submenu);
}

void app_menu_setup_from_directory(GtkMenu * menu, GMenuTreeDirectory * directory) {
	GSList * items = gmenu_tree_directory_get_contents(directory);
	GSList * l;

	for(l = items; l; l = l->next) {
		GMenuTreeItem * item = l->data;
		g_print("new app menu item %d", gmenu_tree_item_get_type(item));
		switch (gmenu_tree_item_get_type (item)) {
		case GMENU_TREE_ITEM_DIRECTORY:
			create_submenu (menu, GMENU_TREE_DIRECTORY (item), NULL);
			break;

		case GMENU_TREE_ITEM_ENTRY:
			create_menuitem (menu, GMENU_TREE_ENTRY (item), NULL);
			break;

		case GMENU_TREE_ITEM_SEPARATOR :
			
			break;

		case GMENU_TREE_ITEM_ALIAS:
			//create_menuitem_from_alias (menu, GMENU_TREE_ALIAS (item));
			break;

		case GMENU_TREE_ITEM_HEADER:
			//create_header (menu, GMENU_TREE_HEADER (item));
			break;

		default:
			break;
		}
	}
	g_slist_free(items);
}

GtkWidget * app_menu_new(){
	GMenuTree * tree = gmenu_tree_lookup("applications.menu", GMENU_TREE_FLAGS_NONE);
	GMenuTreeDirectory * directory = gmenu_tree_get_root_directory (tree);
	GtkWidget * menu = gtk_menu_new();
	app_menu_setup_from_directory(menu, directory);
	g_object_set_data_full( G_OBJECT (menu),
				"menu-tree",
				gmenu_tree_ref(tree),
				(GDestroyNotify) gmenu_tree_unref);
	return menu;
}
