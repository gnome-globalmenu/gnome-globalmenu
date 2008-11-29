#! /bin/bash

source ../patch.sh

#patch '/g_return_if_fail (previous_toplevel != NULL);/d' menushell.c
patch '/gtk_style_copy/i ((GtkWidget*)self)->style = ' menubar.c
