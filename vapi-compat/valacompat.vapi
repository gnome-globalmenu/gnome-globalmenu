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

[CCode (cname = "g_object_class_install_property")]
public static void object_class_install_property  (ObjectClass oclass,
													 uint property_id,
													 ParamSpec pspec);
[CCode (cname = "g_object_add_toggle_ref")]
public static void object_add_toggle_ref(Object object, ToggleNotify notify, void* data);
[CCode (cname = "g_object_remove_toggle_ref")]
public static void object_remove_toggle_ref(Object object, ToggleNotify notify, void* data);
public static delegate void ToggleNotify (void* data, Object object, bool is_last);

public class ParamSpecBoolean : ParamSpec {
	[CCode (cname = "g_param_spec_boolean")]
	public ParamSpecBoolean (string name, string nick, string blurb, bool defaultvalue, ParamFlags flags);
}

}
namespace Gdk {
[CCode (cname = "GDK_WINDOW_XID", cheader_filename="gdk/gdkx.h")]
public static ulong XWINDOW(Gdk.Drawable drawable);
}

[CCode (cprefix = "Gtk", lower_case_cprefix = "gtk_")]
namespace GtkAQD {
	[CCode (cheader_filename = "gtk/gtk.h", cname = "GtkMenuShell")]
	public class MenuShell : Gtk.Container, Atk.Implementor, Gtk.Buildable {
		public GLib.List<Gtk.MenuItem> children;
		public weak Gtk.Widget active_menu_item;
		public weak Gtk.Widget parent_menu_shell;
		public uint button;
		public uint activate_time;
		public uint active;
		public uint have_grab;
		public uint have_xgrab;
		public uint ignore_leave;
		public uint menu_flag;
		public uint ignore_enter;
		public void activate_item (Gtk.Widget menu_item, bool force_deactivate);
		public void append ([CCode (type = "GtkWidget*")] Gtk.MenuItem child);
		public void deselect ();
		public bool get_take_focus ();
		public void prepend (Gtk.Widget child);
		public void select_first (bool search_sensitive);
		public void set_take_focus (bool take_focus);
		[NoWrapper]
		public virtual int get_popup_delay ();
		public virtual void select_item (Gtk.Widget menu_item);
		public bool take_focus { get; set; }
		public virtual signal void activate_current (bool force_hide);
		[HasEmitter]
		public virtual signal void cancel ();
		public virtual signal void cycle_focus (Gtk.DirectionType p0);
		[HasEmitter]
		public virtual signal void deactivate ();
		public virtual signal void move_current (Gtk.MenuDirectionType direction);
		public virtual signal bool move_selected (int distance);
		public virtual signal void selection_done ();
/* Below are in GtkAQD*/
		[HasEmitter]
		public virtual signal void insert(Gtk.Widget widget, int pos);
	}

	[CCode (cheader_filename = "gtk/gtk.h")]
	public class MenuBar : MenuShell, Atk.Implementor, Gtk.Buildable {
		public Gtk.PackDirection get_child_pack_direction ();
		public Gtk.PackDirection get_pack_direction ();
		[CCode (type = "GtkWidget*")]
		public MenuBar ();
		public void set_child_pack_direction (Gtk.PackDirection child_pack_dir);
		public void set_pack_direction (Gtk.PackDirection pack_dir);
		public Gtk.PackDirection child_pack_direction { get; set; }
		public Gtk.PackDirection pack_direction { get; set; }
		[NoAccessorMethod]
		public bool local {get; set;}
	}
}
[CCode (cprefix = "Gtk", lower_case_cprefix = "gtk_")]
namespace GtkCompat {
	[CCode (cheader_filename = "gtk/gtk.h", cname="GtkNotebook")]
	public class Notebook : Gtk.Container, Atk.Implementor, Gtk.Buildable {
		public weak Gtk.NotebookPage cur_page;
		public weak GLib.List children;
		public weak GLib.List first_tab;
		public weak Gtk.Widget menu;
		public weak Gdk.Window event_window;
		public uint timer;
		public uint in_child;
		public uint click_child;
		public uint button;
		public uint need_timer;
		public uint child_has_focus;
		public uint have_visible_child;
		public uint focus_out;
		public uint has_before_previous;
		public uint has_before_next;
		public uint has_after_previous;
		public uint has_after_next;
		public int append_page (Gtk.Widget child, Gtk.Widget? tab_label);
		public int append_page_menu (Gtk.Widget child, Gtk.Widget tab_label, Gtk.Widget menu_label);
		public int get_current_page ();
		public void* get_group ();
		public weak Gtk.Widget get_menu_label (Gtk.Widget child);
		public weak string get_menu_label_text (Gtk.Widget child);
		public int get_n_pages ();
		public weak Gtk.Widget get_nth_page (int page_num);
		public bool get_scrollable ();
		public bool get_show_border ();
		public bool get_show_tabs ();
		public bool get_tab_detachable (Gtk.Widget child);
		public weak Gtk.Widget get_tab_label (Gtk.Widget child);
		public weak string get_tab_label_text (Gtk.Widget child);
		public Gtk.PositionType get_tab_pos ();
		public bool get_tab_reorderable (Gtk.Widget child);
		public int insert_page_menu (Gtk.Widget child, Gtk.Widget tab_label, Gtk.Widget menu_label, int position);
		[CCode (type = "GtkWidget*")]
		public Notebook ();
		public void next_page ();
		public void popup_disable ();
		public void popup_enable ();
		public int prepend_page (Gtk.Widget child, Gtk.Widget tab_label);
		public int prepend_page_menu (Gtk.Widget child, Gtk.Widget tab_label, Gtk.Widget menu_label);
		public void prev_page ();
		public void query_tab_label_packing (Gtk.Widget child, bool expand, bool fill, Gtk.PackType pack_type);
		public void remove_page (int page_num);
		public void reorder_child (Gtk.Widget child, int position);
		public void set_current_page (int page_num);
		public void set_group (void* group);
		public void set_menu_label (Gtk.Widget child, Gtk.Widget menu_label);
		public void set_menu_label_text (Gtk.Widget child, string menu_text);
		public void set_scrollable (bool scrollable);
		public void set_show_border (bool show_border);
		public void set_show_tabs (bool show_tabs);
		public void set_tab_detachable (Gtk.Widget child, bool detachable);
		public void set_tab_label (Gtk.Widget child, Gtk.Widget tab_label);
		public void set_tab_label_packing (Gtk.Widget child, bool expand, bool fill, Gtk.PackType pack_type);
		public void set_tab_label_text (Gtk.Widget child, string tab_text);
		public void set_tab_pos (Gtk.PositionType pos);
		public void set_tab_reorderable (Gtk.Widget child, bool reorderable);
		public static void set_window_creation_hook (Gtk.NotebookWindowCreationFunc func, void* data, GLib.DestroyNotify destroy);
		public virtual int insert_page (Gtk.Widget child, Gtk.Widget tab_label, int position);
		public int page_num(Gtk.Widget child);
		[NoAccessorMethod]
		public bool enable_popup { get; set; }
		public void* group { get; set; }
		[NoAccessorMethod]
		public int group_id { get; set; }
		[NoAccessorMethod]
		public bool homogeneous { get; set; }
		[NoAccessorMethod]
		public int page { get; set; }
		public bool scrollable { get; set; }
		public bool show_border { get; set; }
		public bool show_tabs { get; set; }
		[NoAccessorMethod]
		public uint tab_border { set; }
		[NoAccessorMethod]
		public uint tab_hborder { get; set; }
		public Gtk.PositionType tab_pos { get; set; }
		[NoAccessorMethod]
		public uint tab_vborder { get; set; }
		public virtual signal bool change_current_page (int offset);
		public virtual signal weak Gtk.Notebook create_window (Gtk.Widget page, int x, int y);
		public virtual signal bool focus_tab (Gtk.NotebookTab type);
		public virtual signal void move_focus_out (Gtk.DirectionType direction);
		public virtual signal void page_added (Gtk.Widget p0, uint p1);
		public virtual signal void page_removed (Gtk.Widget p0, uint p1);
		public virtual signal void page_reordered (Gtk.Widget p0, uint p1);
		public virtual signal bool reorder_tab (Gtk.DirectionType direction, bool move_to_last);
		public virtual signal bool select_page (bool move_focus);
		public virtual signal void switch_page (void* page, uint page_num);
	}


}
