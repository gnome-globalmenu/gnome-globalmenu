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
 
#ifndef __GTK_HOTKEY_REGISTRY_H__
#define __GTK_HOTKEY_REGISTRY_H__

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include "gtk-hotkey-info.h"
#include "gtk-hotkey-error.h"

G_BEGIN_DECLS


#define GTK_HOTKEY_TYPE_STORAGE (gtk_hotkey_registry_get_type ())
#define GTK_HOTKEY_REGISTRY(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_HOTKEY_TYPE_STORAGE, GtkHotkeyRegistry))
#define GTK_HOTKEY_REGISTRY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_HOTKEY_TYPE_STORAGE, GtkHotkeyRegistryClass))
#define GTK_HOTKEY_IS_REGISTRY(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_HOTKEY_TYPE_STORAGE))
#define GTK_HOTKEY_IS_REGISTRY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_HOTKEY_TYPE_STORAGE))
#define GTK_HOTKEY_REGISTRY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_HOTKEY_TYPE_STORAGE, GtkHotkeyRegistryClass))

typedef struct _GtkHotkeyRegistry GtkHotkeyRegistry;
typedef struct _GtkHotkeyRegistryClass GtkHotkeyRegistryClass;
typedef struct _GtkHotkeyRegistryPrivate GtkHotkeyRegistryPrivate;

struct _GtkHotkeyRegistry {
	GObject parent;
	GtkHotkeyRegistryPrivate * priv;
};
struct _GtkHotkeyRegistryClass {
	GObjectClass	parent;
	GtkHotkeyInfo*  (*get_hotkey)					(GtkHotkeyRegistry   *self,
													 const char			*app_id,
													 const char			*key_id,
													 GError				**error);
	GList*			(*get_application_hotkeys)		(GtkHotkeyRegistry   *self,
													 const char			*app_id,
													 GError				**error);
	GList*			(*get_all_hotkeys)				(GtkHotkeyRegistry   *self);
	gboolean		(*store_hotkey)					(GtkHotkeyRegistry   *self,
													 GtkHotkeyInfo		*info,
													 GError				**error);
	gboolean		(*delete_hotkey)				(GtkHotkeyRegistry   *self,
													 const gchar		*app_id,
													 const gchar		*key_id,
													 GError				**error);
	gboolean		(*has_hotkey)					(GtkHotkeyRegistry   *self,
													 const gchar		*app_id,
													 const gchar		*key_id);
	void			(*hotkey_stored)				(GtkHotkeyRegistry   *self,
													 GtkHotkeyInfo		*info);
	void			(*hotkey_deleted)				(GtkHotkeyRegistry   *self,
													 GtkHotkeyInfo		*info);
};

GtkHotkeyRegistry*		gtk_hotkey_registry_get_default		(void);

GtkHotkeyInfo*			gtk_hotkey_registry_get_hotkey		(GtkHotkeyRegistry		*self,
															 const char				*app_id,
															 const char				*key_id,
															 GError					**error);
															 
GList*					gtk_hotkey_registry_get_application_hotkeys
															(GtkHotkeyRegistry		*self,
															 const char				*app_id,
															 GError					**error);
															 
GList*					gtk_hotkey_registry_get_all_hotkeys  (GtkHotkeyRegistry		*self);

gboolean				gtk_hotkey_registry_store_hotkey		(GtkHotkeyRegistry		*self,
															 GtkHotkeyInfo			*info,
															 GError					**error);

gboolean				gtk_hotkey_registry_delete_hotkey	(GtkHotkeyRegistry		*self,
															 const gchar			*app_id,
															 const gchar			*key_id,
															 GError					**error);
															 
gboolean				gtk_hotkey_registry_has_hotkey		(GtkHotkeyRegistry		*self,
															 const gchar			*app_id,
															 const gchar			*key_id);

void					gtk_hotkey_registry_hotkey_stored	(GtkHotkeyRegistry		*self,
															 GtkHotkeyInfo			*hotkey);

void					gtk_hotkey_registry_hotkey_deleted	(GtkHotkeyRegistry		*self,
															 GtkHotkeyInfo			*hotkey);

GType					gtk_hotkey_registry_get_type			(void);

G_END_DECLS

#endif
