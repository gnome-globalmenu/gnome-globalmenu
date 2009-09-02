#include <glib.h>
#if GLIB_MINOR_VERSION < 14
G_BEGIN_DECLS
GList * g_hash_table_get_keys (GHashTable *hash_table);
GList * g_hash_table_get_values (GHashTable *hash_table);
G_END_DECLS
#endif
