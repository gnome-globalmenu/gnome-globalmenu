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
 
#ifndef __GTK_HOTKEY_LISTENER_H__
#define __GTK_HOTKEY_LISTENER_H__

#include <glib.h>
#include <glib-object.h>
#include "gtk-hotkey-info.h"

G_BEGIN_DECLS


#define GTK_HOTKEY_TYPE_LISTENER (gtk_hotkey_listener_get_type ())
#define GTK_HOTKEY_LISTENER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_HOTKEY_TYPE_LISTENER, GtkHotkeyListener))
#define GTK_HOTKEY_LISTENER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_HOTKEY_TYPE_LISTENER, GtkHotkeyListenerClass))
#define GTK_HOTKEY_IS_LISTENER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_HOTKEY_TYPE_LISTENER))
#define GTK_HOTKEY_IS_LISTENER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_HOTKEY_TYPE_LISTENER))
#define GTK_HOTKEY_LISTENER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_HOTKEY_TYPE_LISTENER, GtkHotkeyListenerClass))

typedef struct _GtkHotkeyListener GtkHotkeyListener;
typedef struct _GtkHotkeyListenerClass GtkHotkeyListenerClass;
typedef struct _GtkHotkeyListenerPrivate GtkHotkeyListenerPrivate;

struct _GtkHotkeyListener {
	GObject						parent;
	GtkHotkeyListenerPrivate	*priv;
};

struct _GtkHotkeyListenerClass {
	GObjectClass	parent;
	gboolean		(*bind_hotkey)	  (GtkHotkeyListener	*self,
									   GtkHotkeyInfo		*hotkey,
									   GError				**error);
	gboolean		(*unbind_hotkey) (GtkHotkeyListener		*self,
									  GtkHotkeyInfo			*hotkey,
									  GError				**error);
};

GtkHotkeyListener*  gtk_hotkey_listener_get_default			(void);

void				gtk_hotkey_listener_activated			(GtkHotkeyListener	*self,
															 GtkHotkeyInfo		*hotkey,
															 guint				event_time);

gboolean			gtk_hotkey_listener_bind_hotkey		(GtkHotkeyListener  *self,
														 GtkHotkeyInfo		*hotkey,
														 GError				**error);

gboolean			gtk_hotkey_listener_unbind_hotkey   (GtkHotkeyListener  *self,
														 GtkHotkeyInfo		*hotkey,
														 GError				**error);

GType				gtk_hotkey_listener_get_type			(void);

G_END_DECLS

#endif /* __GTK_HOTKEY_LISTENER_H__ */
