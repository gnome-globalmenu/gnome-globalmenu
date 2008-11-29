#! /bin/bash

source ../patch.sh

patch '/gtk_style_copy/i ((GtkWidget*)self)->style = ' menubar.c
patch '/parent_class)->expose_event/s/, &_tmp[0-9])/)/g' menuitem.c window.c
patch '/modify_font/a pango_font_description_free(desc);' menuitem.c
patch '/(GdkNativeWindow)/s/, &_tmp0/, _tmp0/' window.c
