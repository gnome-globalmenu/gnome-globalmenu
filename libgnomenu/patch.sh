#! /bin/bash

source ../patch.sh

patch '/parent_class)->expose_event/s/, &_tmp0), _tmp0/)/g' menushell.c
patch '/static void gnomenu_menu_shell_real_forall.*void\*\ data/{s/GtkCallback/gboolean include_internal, GtkCallback/;s/, void\* data//; }' menushell.c
patch '/static void gnomenu_menu_shell_real_forall/,+8{s/gboolean include_internal;//;s/include_internal = FALSE;//}' menushell.c
patch '/g_return_if_fail (previous_toplevel != NULL);/d' menushell.c
