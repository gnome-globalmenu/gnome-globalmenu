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

#ifndef __GTK_HOTKEY_UTILS_H__
#define __GTK_HOTKEY_UTILS_H__

#include <gio/gio.h>

G_BEGIN_DECLS

#define			gtk_hotkey_g_file_is_directory(file) (gtk_hotkey_g_file_get_type(file) == G_FILE_TYPE_DIRECTORY)
#define			gtk_hotkey_g_file_is_regular(file) (gtk_hotkey_g_file_get_type(file) == G_FILE_TYPE_REGULAR)

GFileType		gtk_hotkey_g_file_get_type			(GFile *file);

G_END_DECLS

#endif /* __GTK_HOTKEY_UTILS_H__ */
