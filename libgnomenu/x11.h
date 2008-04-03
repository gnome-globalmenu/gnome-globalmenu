#ifndef _GNOMENU_X11_
#define _GNOMENU_X11_
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <X11/Xatom.h>
#define _GNOMENU_MESSAGE_TYPE "_GNOMENU_MESSAGE_TYPE"
#define _GNOMENU_METHOD_CALL "_GNOMENU_METHOD_CALL"
#define _GNOMENU_METHOD_RETURN "_GNOMENU_METHOD_RET"

gboolean _send_xclient_message 	( GdkNativeWindow target, gpointer data, guint bytes );
gboolean _peek_xwindow 			( GdkNativeWindow target );
GList * _find_native_by_name		( gchar * name );

gboolean _set_native_buffer ( GdkNativeWindow native, const gchar * buffer_name, guint id, gpointer data, gint bytes);

#define g_queue_for(queue, ele, job) \
		{ GList * _list = (queue)?g_queue_peek_head_link(queue):NULL;\
		  GList * _node; \
		  for(_node = g_list_first(_list); _node; _node=g_list_next(_node)){ \
			ele = _node->data; \
			job; \
		  } \
		}
gpointer _get_native_buffer ( GdkNativeWindow native, const gchar * buffer_name, guint id, gint * bytes, gboolean remove);
gchar * x_prop_name_encode(const char * name, guint id);
gboolean x_prop_name_decode(const char * xpropname, gchar ** name, guint * id);

typedef struct {
	guint id;
	GdkNativeWindow source;
	guint8 type;
} GnomenuXMessage;

#define _XMESSAGE_TYPE_CALL 0
#define _XMESSAGE_TYPE_RETURN 1
#define _XMESSAGE_TYPE_RETURN_VOID 2
#define _XMESSAGE_TYPE_SIGNAL 3
#define _XMESSAGE_TYPE_BIND 4

#endif
