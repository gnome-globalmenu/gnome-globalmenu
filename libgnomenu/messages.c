#include <glib-object.h>
#include "messages.h"

gpointer gnomenu_message_copy(gpointer src){
	return g_memdup(src, sizeof(GnomenuMessage));
}
void gnomenu_message_free(gpointer src){
	g_free(src);
}
GType gnomenu_message_get_type (void) {
  static GType type = 0;

  if (!type)
    type = g_boxed_type_register_static ("GnomenuMessage", 
		gnomenu_message_copy,
		gnomenu_message_free	
	);

  return type;

}
