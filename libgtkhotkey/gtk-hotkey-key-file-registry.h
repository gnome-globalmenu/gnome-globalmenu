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
 
#ifndef __GTK_HOTKEY_KEY_FILE_REGISTRY_H__
#define __GTK_HOTKEY_KEY_FILE_REGISTRY_H__

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include "gtk-hotkey-registry.h"
#include "gtk-hotkey-info.h"

G_BEGIN_DECLS


#define GTK_HOTKEY_TYPE_KEY_FILE_REGISTRY (gtk_hotkey_key_file_registry_get_type ())
#define GTK_HOTKEY_KEY_FILE_REGISTRY(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_HOTKEY_TYPE_KEY_FILE_REGISTRY, GtkHotkeyKeyFileRegistry))
#define GTK_HOTKEY_KEY_FILE_REGISTRY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_HOTKEY_TYPE_KEY_FILE_REGISTRY, GtkHotkeyKeyFileRegistryClass))
#define GTK_HOTKEY_IS_KEY_FILE_REGISTRY(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_HOTKEY_TYPE_KEY_FILE_REGISTRY))
#define GTK_HOTKEY_IS_KEY_FILE_REGISTRY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_HOTKEY_TYPE_KEY_FILE_REGISTRY))
#define GTK_HOTKEY_KEY_FILE_REGISTRY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_HOTKEY_TYPE_KEY_FILE_REGISTRY, GtkHotkeyKeyFileRegistryClass))

typedef struct _GtkHotkeyKeyFileRegistry GtkHotkeyKeyFileRegistry;
typedef struct _GtkHotkeyKeyFileRegistryClass GtkHotkeyKeyFileRegistryClass;
typedef struct _GtkHotkeyKeyFileRegistryPrivate GtkHotkeyKeyFileRegistryPrivate;

struct _GtkHotkeyKeyFileRegistry {
	GtkHotkeyRegistry parent;
	GtkHotkeyKeyFileRegistryPrivate * priv;
};
struct _GtkHotkeyKeyFileRegistryClass {
	GtkHotkeyRegistryClass parent;
};

GType gtk_hotkey_key_file_registry_get_type (void);

G_END_DECLS

#endif
