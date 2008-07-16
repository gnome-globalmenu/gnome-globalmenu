/*
 * This file is part of GtkHotkey.
 * Copyright Mikkel Kamstrup Erlandsen, March, 2008
 *
 *   GtkHotkey is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   GtkHotkey is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with GtkHotkey.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#ifndef __GTK_HOTKEY_INFO_H__
#define __GTK_HOTKEY_INFO_H__

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <stdlib.h>
#include <string.h>

G_BEGIN_DECLS


#define GTK_HOTKEY_TYPE_INFO (gtk_hotkey_info_get_type ())
#define GTK_HOTKEY_INFO(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_HOTKEY_TYPE_INFO, GtkHotkeyInfo))
#define GTK_HOTKEY_INFO_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_HOTKEY_TYPE_INFO, GtkHotkeyInfoClass))
#define GTK_HOTKEY_IS_INFO(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_HOTKEY_TYPE_INFO))
#define GTK_HOTKEY_IS_INFO_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_HOTKEY_TYPE_INFO))
#define GTK_HOTKEY_INFO_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_HOTKEY_TYPE_INFO, GtkHotkeyInfoClass))

typedef struct _GtkHotkeyInfo GtkHotkeyInfo;
typedef struct _GtkHotkeyInfoClass GtkHotkeyInfoClass;
typedef struct _GtkHotkeyInfoPrivate GtkHotkeyInfoPrivate;

struct _GtkHotkeyInfo {
	GObject parent;
	GtkHotkeyInfoPrivate * priv;
};
struct _GtkHotkeyInfoClass {
	GObjectClass parent;
};

gboolean		gtk_hotkey_info_bind (GtkHotkeyInfo* self, GError **error);

gboolean		gtk_hotkey_info_unbind (GtkHotkeyInfo* self, GError **error);

gboolean		gtk_hotkey_info_is_bound (GtkHotkeyInfo* self);

const gchar*	gtk_hotkey_info_get_application_id (GtkHotkeyInfo* self);

const gchar*	gtk_hotkey_info_get_key_id (GtkHotkeyInfo* self);

GAppInfo*		gtk_hotkey_info_get_app_info (GtkHotkeyInfo* self);

const gchar*	gtk_hotkey_info_get_application_id (GtkHotkeyInfo* self);

const gchar*	gtk_hotkey_info_get_signature (GtkHotkeyInfo* self);

const gchar*	gtk_hotkey_info_get_key_id (GtkHotkeyInfo* self);

const gchar*	gtk_hotkey_info_get_description (GtkHotkeyInfo* self);

void			gtk_hotkey_info_set_description (GtkHotkeyInfo* self, const gchar *description);

gboolean		gtk_hotkey_info_equals (GtkHotkeyInfo *hotkey1, GtkHotkeyInfo *hotkey2, gboolean sloppy_equals);

void			gtk_hotkey_info_activated (GtkHotkeyInfo* self, guint event_time);

GtkHotkeyInfo*  gtk_hotkey_info_new						(const gchar	*app_id,
														 const gchar	*key_id,
														 const gchar	*signature,
														 GAppInfo		*app_info);

GType			gtk_hotkey_info_get_type (void);

G_END_DECLS

#endif
