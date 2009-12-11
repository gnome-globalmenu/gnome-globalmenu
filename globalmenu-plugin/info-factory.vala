internal class MenuBarAgentFactory {
	/* Return the unique factory */
	private static MenuBarAgentFactory unique = null;
	private static bool initialized = false;
	public static MenuBarAgentFactory get() {
		assert(initialized);
		return unique;
	}
	public static void init() {
		assert(!initialized);
		unique = new MenuBarAgentFactory();
		initialized = true;
	}
	/* Reset the factory to a clean state,
	 * Restore the attached application into its
	 * untainted state by restoring all function pointers,
	 * removing all signal handlers,
	 * removing all pointers and extra referneces.
	 *
	 * Refer to ~MenuBarAgentFactory()
	 * */
	public static void reset() {
		unique = null;
		initialized = false;
	}

	private List<unowned MenuBarAgent> agent_list;

	internal static void unref_agent(MenuBarAgent * agent) {
		/* This function is called when menubar disposes
		 * MenuBarAgent. Because the MenuBar is no longer
		 * valid after this point, so is the agent.
		 * Therefore we remove it from our internal tracking
		 * list, avoiding freeing it twice when the factory
		 * is diposed.
		 * */
		get().agent_list.remove(agent);
		delete agent;
	}
	private MenuBarAgentFactory() {}
	~MenuBarAgentFactory() {
		foreach(unowned MenuBarAgent agent in agent_list) {
			/*
			 * unset the menubar's reference to the agent object,
			 * so that the agent object will be disposed.
			 * the agent object will clean up after itself.
			 *
			 * Refer to ~MenuBarAgent()
			 * */
			agent.menubar.set_data("globalmenu-agent", null);
		}
	}

	/* Factory Interface */
	internal void associate(Gtk.MenuBar menubar, MenuBarAgent agent) {
		/* This function is called by MenuBarAgent() */
		/* We set a reference of agent object to the menu bar.
		 * So that when the menubar object is destroyed,
		 * the agent object will be disposed.
		 *
		 * By doing this we don't need to deal with
		 * WeakNotify.
		 * */
		menubar.set_data_full("globalmenu-agent", agent, MenuBarAgentFactory.unref_agent);
	}
	public unowned MenuBarAgent create(Gtk.MenuBar menubar) {
		MenuBarAgent* agent = menubar.get_data("globalmenu-agent");
		if(agent != null) return agent;
		agent = new MenuBarAgent(menubar);

		return agent;
	}

	/* Create the menu bar agent object for all menubar widgets
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
