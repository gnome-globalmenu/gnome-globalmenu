#! /bin/bash

source ../patch.sh

patch '/gtk_style_copy/i ((GtkWidget*)self)->style = ' menubar.c
patch '/parent_class)->expose_event/s/, &_tmp0)/)/g' menuitem.c
