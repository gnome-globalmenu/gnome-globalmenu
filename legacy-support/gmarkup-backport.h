#include <glib.h>
#if GLIB_MINOR_VERSION < 16
#include <stdarg.h>
G_BEGIN_DECLS
typedef enum
{
  G_MARKUP_COLLECT_INVALID,
  G_MARKUP_COLLECT_STRING,
  G_MARKUP_COLLECT_STRDUP,
  G_MARKUP_COLLECT_BOOLEAN,
  G_MARKUP_COLLECT_TRISTATE,

  G_MARKUP_COLLECT_OPTIONAL = (1 << 16)
} GMarkupCollectType;

/*any unused number*/
#define G_MARKUP_ERROR_MISSING_ATTRIBUTE 314159

/* useful from start_element */
gboolean   g_markup_collect_attributes (const gchar         *element_name,
                                        const gchar        **attribute_names,
                                        const gchar        **attribute_values,
                                        GError             **error,
                                        GMarkupCollectType   first_type,
                                        const gchar         *first_attr,
                                        ...);

G_END_DECLS
#endif
