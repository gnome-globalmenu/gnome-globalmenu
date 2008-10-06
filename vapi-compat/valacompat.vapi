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

	[CCode (cheader_filename = "gtk/gtk.h")]
	public class MenuItem : Gtk.Item, Atk.Implementor, Gtk.Buildable {
		public weak Gdk.Window event_window;
		public ushort toggle_size;
		public ushort accelerator_width;
		public weak string accel_path;
		public uint show_submenu_indicator;
		public uint submenu_placement;
		public uint submenu_direction;
		public uint right_justify;
		public uint timer_from_keypress;
		public uint from_menubar;
		public uint timer;
		public bool get_right_justified ();
		public weak Gtk.Widget get_submenu ();
		[CCode (type = "GtkWidget*")]
		public MenuItem ();
		[CCode (type = "GtkWidget*")]
		public MenuItem.with_label (string label);
		[CCode (type = "GtkWidget*")]
		public MenuItem.with_mnemonic (string label);
		public void set_accel_path (string accel_path);
		public void set_right_justified (bool right_justified);
		public void set_submenu (Gtk.Widget submenu);
		public Gtk.Menu submenu { get; set; }
		[HasEmitter]
		public virtual signal void activate ();
		public virtual signal void activate_item ();
		[HasEmitter]
		public virtual signal void toggle_size_allocate (int allocation);
		[HasEmitter]
		public virtual signal void toggle_size_request (void* requisition);
		public signal void label_set(Gtk.Label? label);
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
	[CCode (cheader_filename = "gtk/gtk.h")]
	public class Container : Gtk.Widget, Atk.Implementor, Gtk.Buildable {
		public weak Gtk.Widget focus_child;
		public uint need_resize;
		public uint reallocate_redraws;
		public uint has_focus_chain;
		public void add_with_properties (Gtk.Widget widget, ...);
		public void child_get (Gtk.Widget child, ...);
		public void child_get_property (Gtk.Widget child, string property_name, GLib.Value value);
		public void child_get_valist (Gtk.Widget child, string first_property_name, void* var_args);
		public void child_set (Gtk.Widget child, ...);
		public void child_set_property (Gtk.Widget child, string property_name, GLib.Value value);
		public void child_set_valist (Gtk.Widget child, string first_property_name, void* var_args);
		public static weak GLib.ParamSpec class_find_child_property (GLib.ObjectClass cclass, string property_name);
		[CCode (cname = "gtk_container_class_install_child_property")]
		public class void install_child_property (uint property_id, GLib.ParamSpec pspec);
		public static weak GLib.ParamSpec class_list_child_properties (GLib.ObjectClass cclass, uint n_properties);
		public void @foreach (Gtk.Callback callback);
		public uint get_border_width ();
		public weak GLib.List<Gtk.Widget> get_children ();
		public bool get_focus_chain (GLib.List focusable_widgets);
		public weak Gtk.Adjustment get_focus_hadjustment ();
		public weak Gtk.Adjustment get_focus_vadjustment ();
		public Gtk.ResizeMode get_resize_mode ();
		public void propagate_expose (Gtk.Widget child, Gdk.EventExpose event);
		public void resize_children ();
		public void set_border_width (uint border_width);
		public void set_focus_chain (GLib.List focusable_widgets);
		public void set_focus_hadjustment (Gtk.Adjustment adjustment);
		public void set_focus_vadjustment (Gtk.Adjustment adjustment);
		public void set_reallocate_redraws (bool needs_redraws);
		public void set_resize_mode (Gtk.ResizeMode resize_mode);
		public void unset_focus_chain ();
		public virtual GLib.Type child_type ();
		[NoWrapper]
		public virtual weak string composite_name (Gtk.Widget child);
		public virtual void forall (Gtk.Callback callback);
		[NoWrapper]
		public virtual void get_child_property (Gtk.Widget child, uint property_id, GLib.Value value, GLib.ParamSpec pspec);
		[NoWrapper]
		public virtual void set_child_property (Gtk.Widget child, uint property_id, GLib.Value value, GLib.ParamSpec pspec);
		public uint border_width { get; set; }
		[NoAccessorMethod]
		public Gtk.Widget child { set; }
		public Gtk.ResizeMode resize_mode { get; set; }
		[HasEmitter]
		public virtual signal void add (Gtk.Widget widget);
		[HasEmitter]
		public virtual signal void check_resize ();
		[HasEmitter]
		public virtual signal void remove (Gtk.Widget widget);
		[HasEmitter]
		public virtual signal void set_focus_child (Gtk.Widget widget);
	}


}
