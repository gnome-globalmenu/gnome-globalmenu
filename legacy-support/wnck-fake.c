#include <wnck-fake.h>
#ifdef WNCK_USE_FAKE_ACTION_MENU_TYPE
struct _WnckActionMenuClass {
	GObjectClass parent;
};
struct _WnckActionMenu {
	GObject parent;
};

static void wnck_action_menu_class_init (struct _WnckActionMenuClass * klass) {
	WnckScreen * screen = wnck_screen_get_default();
	GList * list = wnck_screen_get_windows(screen);
	g_assert(list != NULL);
	WnckWindow * window = list->data;
	g_assert(window != NULL);
	GtkMenu * menu = wnck_create_window_action_menu(window);
	if(menu != NULL)
		g_object_unref(menu);
}
static void wnck_action_menu_init (GObject * self) {
}

GType wnck_action_menu_get_type_fake (void) {
	static GType wnck_action_menu_type_id = 0;
	if (wnck_action_menu_type_id == 0) {
		static const GTypeInfo g_define_type_info = { 
			sizeof (struct _WnckActionMenuClass), 
			(GBaseInitFunc) NULL, 
			(GBaseFinalizeFunc) NULL, 
			(GClassInitFunc) wnck_action_menu_class_init, 
			(GClassFinalizeFunc) NULL, 
			NULL, 
			sizeof (struct _WnckActionMenu), 
			0, 
			(GInstanceInitFunc) NULL, 
			NULL };
		wnck_action_menu_type_id = g_type_register_static (G_TYPE_OBJECT, "WnckActionMenuFake", &g_define_type_info, 0);
	}
	return wnck_action_menu_type_id;
}
#endif
