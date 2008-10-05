

[CCode (cprefix = "Wnck", lower_case_cprefix = "wnck_")]
namespace WnckCompat {
	[CCode (cheader_filename = "libwnck/libwnck.h")]
	public class Screen : GLib.Object {
		public void calc_workspace_layout (int num_workspaces, int space_index, Wnck.WorkspaceLayout layout);
		public void change_workspace_count (int count);
		public void force_update ();
		public static void free_workspace_layout (Wnck.WorkspaceLayout layout);
		public static weak Screen get (int index);
		public weak Wnck.Window get_active_window ();
		public weak Wnck.Workspace get_active_workspace ();
		public ulong get_background_pixmap ();
		public static weak Screen get_default ();
		public static weak Screen get_for_root (ulong root_window_id);
		public int get_height ();
		public int get_number ();
		public weak Wnck.Window get_previously_active_window ();
		public bool get_showing_desktop ();
		public int get_width ();
		public weak string get_window_manager_name ();
		public weak GLib.List get_windows ();
		public weak GLib.List get_windows_stacked ();
		public weak Wnck.Workspace get_workspace (int workspace);
		public int get_workspace_count ();
		public int get_workspace_index (Wnck.Workspace space);
		public weak Wnck.Workspace get_workspace_neighbor (Wnck.Workspace space, Wnck.MotionDirection direction);
		public weak GLib.List get_workspaces ();
		public void move_viewport (int x, int y);
		public bool net_wm_supports (string atom);
		public void release_workspace_layout (int current_token);
		public void toggle_showing_desktop (bool show);
		public int try_set_workspace_layout (int current_token, int rows, int columns);
		[NoWrapper]
		public virtual void pad2 ();
		[NoWrapper]
		public virtual void pad3 ();
		[NoWrapper]
		public virtual void pad4 ();
		[NoWrapper]
		public virtual void pad5 ();
		[NoWrapper]
		public virtual void pad6 ();
		public virtual signal void active_window_changed (Wnck.Window? previous_window);
		public virtual signal void active_workspace_changed (Wnck.Workspace? previous_workspace);
		public virtual signal void application_closed (Wnck.Application? app);
		public virtual signal void application_opened (Wnck.Application? app);
		public virtual signal void background_changed ();
		public virtual signal void class_group_closed (Wnck.ClassGroup? class_group);
		public virtual signal void class_group_opened (Wnck.ClassGroup? class_group);
		public virtual signal void showing_desktop_changed ();
		public virtual signal void viewports_changed ();
		public virtual signal void window_closed (Wnck.Window? window);
		public virtual signal void window_manager_changed ();
		public virtual signal void window_opened (Wnck.Window? window);
		public virtual signal void window_stacking_changed ();
		public virtual signal void workspace_created (Wnck.Workspace? space);
		public virtual signal void workspace_destroyed (Wnck.Workspace? space);
	}
}
