#include <gtk/gtk.h>
#include <globalmenu-server.h>
static GnomenuMenuItem ** gtk_menu_shell_get_item_array(GtkMenuShell * menu_shell, gint * array_length) {
	GnomenuMenuItem ** rt =
	g_object_get_data(G_OBJECT(menu_shell),
	            "item-array");
	*array_length = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(menu_shell),
				"item-array-length"));
	return rt;
}

static void gtk_menu_shell_set_item_array(GtkMenuShell * menu_shell,
	GnomenuMenuItem ** array, gint array_length) {
	GnomenuMenuItem ** old_array =
	g_object_get_data(G_OBJECT(menu_shell),
	            "item-array");
	if(old_array != array) {
		g_object_set_data_full(G_OBJECT(menu_shell),
					"item-array", array, g_free);
	}
	g_object_set_data(G_OBJECT(menu_shell),
				"item-array-length", GINT_TO_POINTER(array_length));
}


/**
 * only removes the extra reference for later attached widgets.
 */
void gtk_menu_shell_remove_all(GtkMenuShell * menu_shell) {
	int array_length = 0;
	int i;
	GnomenuMenuItem **array = gtk_menu_shell_get_item_array(menu_shell, &array_length);
	for(i = 0; i < array_length; i++) {
		g_object_unref(array[i]);
	}
	gtk_menu_shell_set_item_array(menu_shell, NULL, 0);
}

int gtk_menu_shell_get_length(GtkMenuShell * menu_shell) {
	int array_length = 0;
	GnomenuMenuItem ** array = gtk_menu_shell_get_item_array(menu_shell, &array_length);
	int length = array_length;
	int i;
	for(i = array_length - 1; i >= 0; i--) {
		if(gnomenu_menu_item_get_truncated(array[i])) length --;
	}
	return length;
}
/**
 * Ensures the menu shell has 'length' elements
 * If it had more elements, set 'truncated' flag on the extra ones.
 * non-gnomenu-item is not counted.
 * */
void gtk_menu_shell_set_length(GtkMenuShell * menu_shell, gint length) {
	int i = 0;
	int array_length = 0;
	GnomenuMenuItem **array = gtk_menu_shell_get_item_array(menu_shell, &array_length);
	if( array_length < length) {
		GnomenuMenuItem ** new_array = g_new0(GnomenuMenuItem*, length);
		/* grow the array for fast indexing items */
		/* first setup the existing items */
		for(i = 0; i < array_length; i++) {
			new_array[i] = array[i];
		}
		/* then create the new items and store an unrefed pointer
           to the array, too.*/
		for(i = array_length; i < length; i++) {
			GnomenuMenuItem * item = gnomenu_menu_item_new();
			new_array[i] = g_object_ref_sink(item);
			gtk_menu_shell_append(menu_shell, GTK_WIDGET(item));
		}
		array = new_array;
		array_length = length;
		/* Recalculate the children list, pass through the next
		   step. This is subopti!*/
		gtk_menu_shell_set_item_array(menu_shell, array, array_length);
	}
	
	/* set the truncated flags on the children */
	for(i = 0; i < array_length; i++) {
		if(i >= length) {
			gnomenu_menu_item_set_truncated(array[i], TRUE);
		} else {
			gnomenu_menu_item_set_truncated(array[i], FALSE);
		}
	}
}

GtkMenuItem * gtk_menu_shell_get_item(GtkMenuShell * menu_shell, gint position) {
	GnomenuMenuItem ** array = NULL;
	gint length = 0;
	array = gtk_menu_shell_get_item_array(menu_shell, &length);
	if(position >= length) {
		/* grow as needed */
		gtk_menu_shell_set_length(menu_shell, position + 1);
		array = gtk_menu_shell_get_item_array(menu_shell, &length);
	}
	if(position == -1) position = length - 1;

	return GTK_MENU_ITEM(array[position]);
}

gint gtk_menu_shell_get_item_position(GtkMenuShell *menu_shell, GtkMenuItem * item) {
	GnomenuMenuItem ** array = NULL;
	gint length = 0;
	gint i = 0;
	array = gtk_menu_shell_get_item_array(menu_shell, &length);
	if(array == NULL) return -1;
	for(i = 0; i < length; i++) {
		if(GTK_MENU_ITEM(array[i]) == item) return i;
	}
	return -1;
}
