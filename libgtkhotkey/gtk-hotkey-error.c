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

#include "gtk-hotkey-error.h"

GQuark
gtk_hotkey_listener_error_quark (void)
{
  return g_quark_from_static_string ("gtk-hotkey-listener-error-quark");
}

GQuark
gtk_hotkey_registry_error_quark (void)
{
  return g_quark_from_static_string ("gtk-hotkey-storage-error-quark");
}
