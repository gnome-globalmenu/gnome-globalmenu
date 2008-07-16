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
 
#include <gio/gio.h>

GFileType
gtk_hotkey_g_file_get_type (GFile *file)
{
	GFileInfo   *info;
	GFileType   type;
	GError		*error;
	
	g_return_val_if_fail (G_IS_FILE(file), G_FILE_TYPE_UNKNOWN);
	
	if (!g_file_query_exists(file, NULL))
		return G_FILE_TYPE_UNKNOWN;
	
	g_return_val_if_fail (G_IS_FILE(file), G_FILE_TYPE_UNKNOWN);
	error = NULL;
	info = g_file_query_info (file, G_FILE_ATTRIBUTE_STANDARD_TYPE,
							  0, NULL, &error);
	
	if (error) {
		g_critical ("Failed to create GFileInfo: %s", error->message);
		g_error_free (error);
		return G_FILE_TYPE_UNKNOWN;
	}
	
	type = g_file_info_get_file_type (info);
	g_object_unref (info);
	
	return type;
}
