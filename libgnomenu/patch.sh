#! /bin/bash

source ../patch.sh
if [ $1 == 'backup' ]; then
backup menuitem.c globalmenu.c monitor.c menubar.c menubarbox.c label.c
exit 0;
fi;

patch '/g_return_if_fail.*old_parent/d' menuitem.c
patch '/g_return_if_fail.*previous_screen/d' menuitem.c globalmenu.c
patch '/g_return_if_fail (previous_window != NULL)/d' monitor.c

patch 's/gboolean include_internals;//g;s/include_internals = FALSE;//g' menuitem.c menubar.c menubarbox.c label.c
patch 's/GtkCallback callback, void\* callback_target, void\* data/gboolean include_internals, GtkCallback callback, void* callback_target/g' menuitem.c menubar.c menubarbox.c label.c
patch 's/callback, callback_target, data/include_internals, callback, callback_target/g' menuitem.c menubar.c
`which patch` -p0 << EOF
--- interface-item.h	2009-02-16 22:32:40.000000000 -0500
+++ interface-item.h.new	2009-02-16 22:37:38.000000000 -0500
@@ -10,6 +10,22 @@
 
 G_BEGIN_DECLS
 
+typedef enum  {
+	GNOMENU_ITEM_TYPE_NORMAL = 0,
+	GNOMENU_ITEM_TYPE_CHECK = 1,
+	GNOMENU_ITEM_TYPE_RADIO = 2,
+	GNOMENU_ITEM_TYPE_IMAGE = 3,
+	GNOMENU_ITEM_TYPE_SEPARATOR = 4,
+	GNOMENU_ITEM_TYPE_ARROW = 5,
+	GNOMENU_ITEM_TYPE_ICON = 6
+} GnomenuItemType;
+
+typedef enum  {
+	GNOMENU_ITEM_STATE_UNTOGGLED = 0,
+	GNOMENU_ITEM_STATE_TOGGLED = 1,
+	GNOMENU_ITEM_STATE_TRISTATE = 2
+} GnomenuItemState;
+
 
 #define GNOMENU_TYPE_ITEM (gnomenu_item_get_type ())
 #define GNOMENU_ITEM(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOMENU_TYPE_ITEM, GnomenuItem))
@@ -49,22 +65,6 @@
 	void (*set_item_font) (GnomenuItem* self, const char* value);
 };
 
-typedef enum  {
-	GNOMENU_ITEM_TYPE_NORMAL = 0,
-	GNOMENU_ITEM_TYPE_CHECK = 1,
-	GNOMENU_ITEM_TYPE_RADIO = 2,
-	GNOMENU_ITEM_TYPE_IMAGE = 3,
-	GNOMENU_ITEM_TYPE_SEPARATOR = 4,
-	GNOMENU_ITEM_TYPE_ARROW = 5,
-	GNOMENU_ITEM_TYPE_ICON = 6
-} GnomenuItemType;
-
-typedef enum  {
-	GNOMENU_ITEM_STATE_UNTOGGLED = 0,
-	GNOMENU_ITEM_STATE_TOGGLED = 1,
-	GNOMENU_ITEM_STATE_TRISTATE = 2
-} GnomenuItemState;
-
 
 GType gnomenu_item_type_get_type (void);
 GType gnomenu_item_state_get_type (void);
EOF
if pkg-config --exists 'libwnck-1.0 < 2.20' ; then
patch '/static void _gnomenu_monitor_on_active_window_changed_wnck_screen_active_window_changed/s;WnckWindow\* previous_window, ;;' monitor.c
patch '/gnomenu_monitor_on_active_window_changed/s;self, _sender, previous_window;self, _sender, NULL;' monitor.c
fi;
if pkg-config --exists 'gtk+-2.0 < 2.12' ; then
patch 's;gtk_menu_item_set_submenu ((GtkMenuItem\*) self, NULL);gtk_menu_item_remove_submenu ((GtkMenuItem*) self);' menuitem.c
fi
check_restore menuitem.c globalmenu.c monitor.c menubar.c menubarbox.c label.c
