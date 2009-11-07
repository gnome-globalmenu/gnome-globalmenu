internal class MenuBarInfoFactory {
	/* Return the unique factory */
	private static MenuBarInfoFactory unique = null;
	private static bool initialized = false;
	public static MenuBarInfoFactory get() {
		assert(initialized);
		return unique;
	}
	public static void init() {
		assert(!initialized);
		unique = new MenuBarInfoFactory();
		initialized = true;
	}
	/* Reset the factory to a clean state,
	 * Restore the attached application into its
	 * untainted state by restoring all function pointers,
	 * removing all signal handlers,
	 * removing all pointers and extra referneces.
	 *
	 * Refer to ~MenuBarInfoFactory()
	 * */
	public static void reset() {
		unique = null;
		initialized = false;
	}

	private List<unowned MenuBarInfo> info_list;

	internal static void unref_info(MenuBarInfo * info) {
		/* This function is called when menubar disposes
		 * MenuBarInfo. Because the MenuBar is no longer
		 * valid after this point, so is the info.
		 * Therefore we remove it from our internal tracking
		 * list, avoiding freeing it twice when the factory
		 * is diposed.
		 * */
		get().info_list.remove(info);
		delete info;
	}
	private MenuBarInfoFactory() {}
	~MenuBarInfoFactory() {
		foreach(unowned MenuBarInfo info in info_list) {
			/*
			 * unset the menubar's reference to the info object,
			 * so that the info object will be disposed.
			 * the info object will clean up after itself.
			 *
			 * Refer to ~MenuBarInfo()
			 * */
			info.menubar.set_data("globalmenu-info", null);
		}
	}

	/* Factory Interface */
	internal void associate(Gtk.MenuBar menubar, MenuBarInfo info) {
		/* This function is called by MenuBarInfo() */
		/* We set a reference of info object to the menu bar.
		 * So that when the menubar object is destroyed,
		 * the info object will be disposed.
		 *
		 * By doing this we don't need to deal with
		 * WeakNotify.
		 * */
		menubar.set_data_full("globalmenu-info", info, MenuBarInfoFactory.unref_info);
	}
	public unowned MenuBarInfo create(Gtk.MenuBar menubar) {
		MenuBarInfo* info = menubar.get_data("globalmenu-info");
		if(info != null) return info;
		info = new MenuBarInfo(menubar);

		return info;
	}

	/* Create the menu bar info object for all menubar widgets
	 * currently attached to any toplevel */
	public void prepare_attached_menubars() {
		List<weak Gtk.Widget> toplevels = Gtk.Window.list_toplevels();
		foreach(var toplevel in toplevels) {
			prepare_attached_menubars_r(toplevel);
		}
	}

	private void prepare_attached_menubars_r(Gtk.Widget parent) {
		if(parent is Gtk.MenuBar) {
			create(parent as Gtk.MenuBar);
		} else if(parent is Gtk.Container) {
			List<weak Gtk.Widget> children = (parent as Gtk.Container).get_children();
			foreach(var widget in children) {
				prepare_attached_menubars_r(widget);
			}
		}
	}
}
