namespace GLib {
[Compact]
[CCode (cname = "GStringChunk", cprefix = "g_string_chunk_", free_function = "g_string_chunk_free")]
public class StringChunk {
	public StringChunk (ulong size);
	public weak string insert(string str);
	public weak string insert_const(string str);
	public weak string insert_len(void * buffer, ulong len);
	public void clear();
}
[CCode (cname = "g_object_add_toggle_ref")]
public static void object_add_toggle_ref(Object object, ToggleNotify notify, void* data);
[CCode (cname = "g_object_remove_toggle_ref")]
public static void object_remove_toggle_ref(Object object, ToggleNotify notify, void* data);
public static delegate void ToggleNotify (void* data, Object object, bool is_last);
}
namespace Gdk {
[CCode (cname = "GDK_WINDOW_XID", cheader_filename="gdk/gdkx.h")]
public static ulong XWINDOW(Gdk.Drawable drawable);
}
