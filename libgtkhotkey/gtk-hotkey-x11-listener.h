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
 
#ifndef __GTK_HOTKEY_X11_LISTENER_H__
#define __GTK_HOTKEY_X11_LISTENER_H__

#include <glib.h>
#include <glib-object.h>
#include "gtk-hotkey-listener.h"
#include "gtk-hotkey-info.h"

G_BEGIN_DECLS


#define GTK_HOTKEY_TYPE_X11_LISTENER (gtk_hotkey_x11_listener_get_type ())
#define GTK_HOTKEY_X11_LISTENER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_HOTKEY_TYPE_X11_LISTENER, GtkHotkeyX11Listener))
#define GTK_HOTKEY_X11_LISTENER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_HOTKEY_TYPE_X11_LISTENER, GtkHotkeyX11ListenerClass))
#define GTK_HOTKEY_IS_X11_LISTENER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_HOTKEY_TYPE_X11_LISTENER))
#define GTK_HOTKEY_IS_X11_LISTENER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_HOTKEY_TYPE_X11_LISTENER))
#define GTK_HOTKEY_X11_LISTENER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_HOTKEY_TYPE_X11_LISTENER, GtkHotkeyX11ListenerClass))

typedef struct _GtkHotkeyX11Listener GtkHotkeyX11Listener;
typedef struct _GtkHotkeyX11ListenerClass GtkHotkeyX11ListenerClass;
typedef struct _GtkHotkeyX11ListenerPrivate GtkHotkeyX11ListenerPrivate;

struct _GtkHotkeyX11Listener {
	GtkHotkeyListener parent;
	GtkHotkeyX11ListenerPrivate * priv;
};
struct _GtkHotkeyX11ListenerClass {
	GtkHotkeyListenerClass parent;
};

GType gtk_hotkey_x11_listener_get_type (void);

G_END_DECLS

#endif
