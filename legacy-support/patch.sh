#! /bin/bash

source ../patch.sh

if pkg-config --exists 'libwnck-1.0 < 2.20' ; then
patch '/static void _gnomenu_monitor_on_active_window_changed_wnck_screen_active_window_changed/s;WnckWindow\* previous_window, ;;' monitor.c
patch '/gnomenu_monitor_on_active_window_changed/s;self, _sender, previous_window;self, _sender, NULL;' monitor.c
fi;
if pkg-config --exists 'gtk+-2.0 < 2.12' ; then
patch 's;gtk_menu_item_set_submenu ((GtkMenuItem\*) self, NULL);gtk_menu_item_remove_submenu ((GtkMenuItem*) self);' menuitem.c
fi
