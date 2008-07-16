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

#ifndef __GTK_HOTKEY_ERROR_H__
#define __GTK_HOTKEY_ERROR_H__

#include <glib/gquark.h>

G_BEGIN_DECLS

/**
 * GTK_HOTKEY_LISTENER_ERROR:
 *  
 * Error domain for #GtkHotkeyListener.
 */
#define GTK_HOTKEY_LISTENER_ERROR gtk_hotkey_listener_error_quark()
GQuark gtk_hotkey_listener_error_quark (void);

/**
 * GTK_HOTKEY_REGISTRY_ERROR:
 *  
 * Error domain for #GtkHotkeyRegistry.
 */
#define GTK_HOTKEY_REGISTRY_ERROR gtk_hotkey_registry_error_quark()
GQuark gtk_hotkey_registry_error_quark (void);

/**
 * GtkHotkeyListenerError:
 * @GTK_HOTKEY_LISTENER_ERROR_BIND: An error occured when binding a hotkey with
 *                                  the listener
 * @GTK_HOTKEY_LISTENER_ERROR_UNBIND: An error occured when unbinding a hotkey 
 *                                    with the listener
 * 
 * Error codes for #GError<!-- -->s related to #GtkHotkeyListener<!-- -->s
 */
typedef enum
{
	GTK_HOTKEY_LISTENER_ERROR_BIND,
	GTK_HOTKEY_LISTENER_ERROR_UNBIND,
} GtkHotkeyListenerError;

/**
 * GtkHotkeyRegistryError:
 * @GTK_HOTKEY_REGISTRY_ERROR_UNKNOWN_APP: The application which is the involved
 *                                         in the transaction has not registered
 *                                         any hotkeys
 * @GTK_HOTKEY_REGISTRY_ERROR_UNKNOWN_KEY: The hotkey key-id (the identifier that
 *                                         identifies the hotkey among those 
 *                                         belonging to an application) is not
 *                                         known.
 * @GTK_HOTKEY_REGISTRY_ERROR_MALFORMED_MEDIUM: The medium from which to read
 *                                              or write is in an unrecoverable
 *                                              state. For example a file
 *                                              containing syntax errors
 * @GTK_HOTKEY_REGISTRY_ERROR_IO: There was some problem with the actual io
 *                                operation. Missing file permissions, disk full,
 *                                etc.
 * @GTK_HOTKEY_REGISTRY_ERROR_UNKNOWN: Unexpected and uncharacterized error
 * @GTK_HOTKEY_REGISTRY_ERROR_BAD_SIGNATURE: The hotkey signature is not valid.
 *                                           See #GtkHotkeyInfo:signature.
 * @GTK_HOTKEY_REGISTRY_ERROR_MISSING_APP: A #GtkHotkeyInfo is referring an
 *                                         application via its #GtkHotkeyInfo:app-info
 *                                         property, but the application can not
 *                                         be found.
 * 
 * Error codes for #GError<!-- -->s related to #GtkHotkeyRegistry<!-- -->s
 */
typedef enum
{
	GTK_HOTKEY_REGISTRY_ERROR_UNKNOWN_APP,
	GTK_HOTKEY_REGISTRY_ERROR_UNKNOWN_KEY,
	GTK_HOTKEY_REGISTRY_ERROR_MALFORMED_MEDIUM,
	GTK_HOTKEY_REGISTRY_ERROR_IO,
	GTK_HOTKEY_REGISTRY_ERROR_UNKNOWN,
	GTK_HOTKEY_REGISTRY_ERROR_BAD_SIGNATURE,
	GTK_HOTKEY_REGISTRY_ERROR_MISSING_APP,
} GtkHotkeyRegistryError;

G_END_DECLS

#endif /* __GTK_HOTKEY_ERROR_H__ */
