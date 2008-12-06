#! /bin/bash

source ../patch.sh

patch '/gtk_style_copy/i ((GtkWidget*)self)->style = ' menubar.c
patch '/parent_class)->expose_event/s/, &_tmp[0-9])/)/g' menuitem.c window.c
patch '/modify_font/a pango_font_description_free(desc);' menuitem.c
patch '/(GdkNativeWindow)/s/, &_tmp0/, _tmp0/' window.c
patch '/g_return_if_fail.*old_parent/d' menuitem.c
patch 's/gboolean include_internals;//g;s/include_internals = FALSE;//g' menuitem.c
patch 's/GtkCallback callback, void\* callback_target, void\* data/gboolean include_internals, GtkCallback callback, void* callback_target/g' menuitem.c
patch 's/callback, callback_target, data/include_internals, callback, callback_target/g' menuitem.c
