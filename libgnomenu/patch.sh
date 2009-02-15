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

if [ pkg-config --exists 'libwnck-1.0 < 2.20' ]; then
patch '/static void _gnomenu_monitor_on_active_window_changed_wnck_screen_active_window_changed/s;WnckWindow* previous_window, ;;' monitor.c
patch '/gnomenu_monitor_on_active_window_changed/s;self, _sender, previous_window;self, _sender, NULL;' monitor.c
fi;

check_restore menuitem.c globalmenu.c monitor.c menubar.c menubarbox.c label.c
