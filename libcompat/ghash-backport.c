#include <glib.h>
#if GLIB_MINOR_VERSION < 14
#include "gmarkup-backport.h"

static void ghtgv_foreach(gpointer key, gpointer value, gpointer data) {
	GList ** list = data;
	*list = g_list_prepend(*list, value);
}
static void ghtgk_foreach(gpointer key, gpointer value, gpointer data) {
	GList ** list = data;
	*list = g_list_prepend(*list, key);
}

/**
 * g_hash_table_get_values:
 * @hash_table: a #GHashTable
 *
 * Retrieves every value inside @hash_table. The returned data is
 * valid until @hash_table is modified.
 *
 * Return value: a #GList containing all the values inside the hash
 *   table. The content of the list is owned by the hash table and
 *   should not be modified or freed. Use g_list_free() when done
 *   using the list.
 *
 * This function differs from the original GLib implementation,
 * since here we don't have access to the private members.
 */
GList *
g_hash_table_get_values (GHashTable *hash_table)
{
	GList * ret = NULL;
	g_hash_table_foreach(hash_table, ghtgv_foreach, &ret);
	return g_list_reverse(ret);
}
/**
 * g_hash_table_get_keys:
 * @hash_table: a #GHashTable
 *
 * Retrieves every key inside @hash_table. The returned data is valid
 * until @hash_table is modified.
 *
 * Return value: a #GList containing all the keys inside the hash
 *   table. The content of the list is owned by the hash table and
 *   should not be modified or freed. Use g_list_free() when done
 *   using the list.
 *
 * This function differs from the original GLib implementation,
 * since here we don't have access to the private members.
 */
GList *
g_hash_table_get_keys (GHashTable *hash_table)
{
    GList * ret = NULL;
    g_hash_table_foreach(hash_table, ghtgk_foreach, &ret);
    return g_list_reverse(ret);
}

#endif
