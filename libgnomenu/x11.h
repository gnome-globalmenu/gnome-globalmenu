#ifndef _GNOMENU_X11_
#define _GNOMENU_X11_
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <X11/Xatom.h>
#define _GNOMENU_MESSAGE_TYPE gdk_atom_intern("_GNOMENU_MESSAGE_TYPE", FALSE)
#define _GNOMENU_METHOD_CALL gdk_atom_intern("_GNOMENU_METHOD_CALL", FALSE)
#define _GNOMENU_METHOD_RET gdk_atom_intern("_GNOMENU_METHOD_RET", FALSE)

gboolean _send_xclient_message 	( GdkNativeWindow target, gpointer data, guint bytes );
gboolean _peek_xwindow 			( GdkNativeWindow target );
GList * _find_native_by_name		( gchar * name );

gboolean _set_native_buffer ( GdkNativeWindow native, GdkAtom buffer, gpointer data, gint bytes);

#define g_queue_for(queue, ele, job) \
		{ GList * _list = (queue)?g_queue_peek_head_link(queue):NULL;\
		  GList * _node; \
		  for(_node = g_list_first(_list); _node; _node=g_list_next(_node)){ \
			ele = _node->data; \
			job; \
		  } \
		}
gpointer _get_native_buffer ( GdkNativeWindow native, GdkAtom buffer, gint * bytes, gboolean remove);
#endif _GNOMENU_X11_
