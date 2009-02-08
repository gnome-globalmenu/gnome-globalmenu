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

check_restore menuitem.c globalmenu.c monitor.c menubar.c menubarbox.c label.c
