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
#include "gtk-hotkey-x11-listener.h"
#include "gtk-hotkey-listener.h"
#include "gtk-hotkey-info.h"
#include "x11/tomboykeybinder.h"

struct _GtkHotkeyX11ListenerPrivate {
	GList   *hotkeys;
};

enum  {
	GTK_HOTKEY_X11_LISTENER_DUMMY_PROPERTY
};
static gboolean	gtk_hotkey_x11_listener_real_bind_hotkey	(GtkHotkeyListener  *base,
															 GtkHotkeyInfo		*hotkey,
															 GError				**error);

static gboolean	gtk_hotkey_x11_listener_real_unbind_hotkey  (GtkHotkeyListener  *base,
															 GtkHotkeyInfo		*hotkey,
															 GError				**error);

static void		hotkey_activated_cb								(char				*signature,
																 gpointer			user_data);

static GtkHotkeyInfo*
				find_hotkey_from_key_id							(GtkHotkeyX11Listener	*self,
																 const gchar			*key_id);

static gpointer gtk_hotkey_x11_listener_parent_class = NULL;


/**
 * SECTION:gtk-hotkey-x11-listener
 * @short_description: Implementation of #GtkHotkeyListener for a standard X11
 *                     environment
 * @see_also: #GtkHotkeyRegistry, #GtkHotkeyInfo
 *
 * This implementation of a #GtkHotkeyListener should work in any X11
 * environment.
 **/	

static gboolean
gtk_hotkey_x11_listener_real_bind_hotkey (GtkHotkeyListener *base,
											  GtkHotkeyInfo		*hotkey,
											  GError			**error)
{
	GtkHotkeyX11Listener	*self;
	
	g_return_val_if_fail (GTK_HOTKEY_IS_X11_LISTENER(base), FALSE);
	g_return_val_if_fail (GTK_HOTKEY_IS_INFO (hotkey), FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);
	
	self = GTK_HOTKEY_X11_LISTENER (base);
	
	if (find_hotkey_from_key_id(self, 
								gtk_hotkey_info_get_key_id (hotkey))) {
		g_warning ("Hotkey '%s' already registered. Ignoring register request.",
				   gtk_hotkey_info_get_key_id (hotkey));
		return FALSE;
	}
	
	if (tomboy_keybinder_bind (gtk_hotkey_info_get_signature(hotkey),
							   hotkey_activated_cb,
							   self)) {
		self->priv->hotkeys = g_list_prepend (self->priv->hotkeys, hotkey);
		g_object_ref (hotkey);
		g_signal_connect_swapped (self, "activated",
								  G_CALLBACK(gtk_hotkey_info_activated), hotkey);
		return TRUE;
	}
	
	/* Bugger, we failed to bind */
	g_set_error (error, GTK_HOTKEY_LISTENER_ERROR,
				 GTK_HOTKEY_LISTENER_ERROR_BIND,
				 "Failed to register hotkey '%s' with signature '%s'",
				 gtk_hotkey_info_get_key_id (hotkey),
				 gtk_hotkey_info_get_signature (hotkey));
	
	return FALSE;
}


static gboolean
gtk_hotkey_x11_listener_real_unbind_hotkey (GtkHotkeyListener   *base,
												GtkHotkeyInfo		*hotkey,
												GError				**error)
{
	GtkHotkeyX11Listener	*self;
	GtkHotkeyInfo			*saved_hk;
	const gchar				*signature;
	
	g_return_val_if_fail (GTK_HOTKEY_IS_X11_LISTENER (base), FALSE);
	g_return_val_if_fail (GTK_HOTKEY_IS_INFO (hotkey), FALSE);
	
	self = GTK_HOTKEY_X11_LISTENER (base);
	signature = gtk_hotkey_info_get_signature (hotkey);
	saved_hk = find_hotkey_from_key_id (self, gtk_hotkey_info_get_key_id(hotkey));
	
	if (!saved_hk) {
		g_set_error (error, GTK_HOTKEY_LISTENER_ERROR,
					 GTK_HOTKEY_LISTENER_ERROR_UNBIND,
					 "Failed to unregister hotkey '%s' with signature '%s'. "
					 "No hotkey with that signature is known",
					 gtk_hotkey_info_get_key_id (hotkey),
					 signature);
		return FALSE;
	}
	
	/* Remove actual keybinding */
	tomboy_keybinder_unbind (signature, hotkey_activated_cb);
	
	/* Clean up refs */
	self->priv->hotkeys = g_list_remove (self->priv->hotkeys, saved_hk);
	g_object_unref (saved_hk);
	
	/* Clean up signal handler */
	gulong handler = g_signal_handler_find (self,
											G_SIGNAL_MATCH_DATA | G_SIGNAL_MATCH_FUNC,
											0, 0, NULL, gtk_hotkey_info_activated,
											hotkey);
	if (handler == 0) {
		g_warning ("Failed to disconnect signal handler for hotkey '%s'",
				   gtk_hotkey_info_get_key_id (hotkey));
	} else {
		g_signal_handler_disconnect (self, handler);
	}
	
	return TRUE;
											
}

static void
hotkey_activated_cb	(char		   *signature,
					 gpointer		user_data)
{
	GtkHotkeyX11Listener	*self;
	GtkHotkeyInfo			*hotkey;
	GList					*iter;
	guint					event_time;
	
	g_return_if_fail (GTK_HOTKEY_IS_X11_LISTENER(user_data));
	g_return_if_fail (signature != NULL);
	
	self = GTK_HOTKEY_X11_LISTENER(user_data);
	event_time = tomboy_keybinder_get_current_event_time ();
	
	for (iter = self->priv->hotkeys; iter; iter = iter->next) {
		hotkey = GTK_HOTKEY_INFO (iter->data); 
		gtk_hotkey_listener_activated (GTK_HOTKEY_LISTENER(self),
									   hotkey, event_time);
	}
	
}

static GtkHotkeyInfo*
find_hotkey_from_key_id (GtkHotkeyX11Listener	*self,
						 const gchar			*key_id)
{
	GList			*iter;
	GtkHotkeyInfo   *hotkey;
	
	g_return_val_if_fail (GTK_HOTKEY_IS_X11_LISTENER(self), NULL);
	g_return_val_if_fail (key_id != NULL, NULL);
	
	for (iter = self->priv->hotkeys; iter; iter = iter->next) {
		hotkey = GTK_HOTKEY_INFO (iter->data);
		
		if (g_str_equal (gtk_hotkey_info_get_key_id(hotkey), key_id))
			return hotkey;
	}
	
	return NULL;
}

static void
gtk_hotkey_x11_listener_class_init (GtkHotkeyX11ListenerClass * klass)
{
	gtk_hotkey_x11_listener_parent_class = g_type_class_peek_parent (klass);
	
	GTK_HOTKEY_LISTENER_CLASS (klass)->bind_hotkey =
								gtk_hotkey_x11_listener_real_bind_hotkey;
	GTK_HOTKEY_LISTENER_CLASS (klass)->unbind_hotkey =
								gtk_hotkey_x11_listener_real_unbind_hotkey;
	
	/* Initialize the tomboy keybinder */
	tomboy_keybinder_init ();
}


static void
gtk_hotkey_x11_listener_init (GtkHotkeyX11Listener * self)
{
	self->priv = g_new0 (GtkHotkeyX11ListenerPrivate, 1);
}

static void
gtk_hotkey_x11_listener_finalize (GtkHotkeyX11Listener * self)
{
	g_free(self->priv);
}

GType
gtk_hotkey_x11_listener_get_type (void)
{
	static GType gtk_hotkey_x11_listener_type_id = 0;
	
	if (G_UNLIKELY (gtk_hotkey_x11_listener_type_id == 0)) {
		static const GTypeInfo g_define_type_info = {
			sizeof (GtkHotkeyX11ListenerClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) gtk_hotkey_x11_listener_finalize,
			(GClassInitFunc) gtk_hotkey_x11_listener_class_init,
			(GClassFinalizeFunc) NULL,
			NULL,
			sizeof (GtkHotkeyX11Listener),
			0,
			(GInstanceInitFunc) gtk_hotkey_x11_listener_init
		};
		
		gtk_hotkey_x11_listener_type_id = g_type_register_static (GTK_HOTKEY_TYPE_LISTENER, "GtkHotkeyX11Listener", &g_define_type_info, 0);
	}
	return gtk_hotkey_x11_listener_type_id;
}
