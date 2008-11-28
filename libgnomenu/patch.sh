#! /bin/bash


function patch {
	edit=$1;
	shift;
	for i in $*; do
		echo -n patching $i with $edit ...
		if ! sed -e "$edit" $i > $i.new; then
			echo failed.
		else
			if cmp $i $i.new; then
				echo noting was done.
			else 
				echo OK.
			fi;
		fi
		mv $i.new $i; 
	done;
}

patch '/parent_class)->expose_event/s/, &_tmp0), _tmp0/)/g' menushell.c
patch '/static void gnomenu_menu_shell_real_forall.*void\*\ data/{s/GtkCallback/gboolean include_internal, GtkCallback/;s/, void\* data//; }' menushell.c
patch '/static void gnomenu_menu_shell_real_forall/,+8{s/gboolean include_internal;//;s/include_internal = FALSE;//}' menushell.c
patch '/g_return_if_fail (previous_toplevel != NULL);/d' menushell.c
