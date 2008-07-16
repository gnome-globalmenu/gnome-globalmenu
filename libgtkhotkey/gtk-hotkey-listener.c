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
 
#include "gtk-hotkey-listener.h"
#include "gtk-hotkey-x11-listener.h"
#include "gtk-hotkey-marshal.h"

/* FIXME: The default listener is hardcoded to x11, should be compilation target dependent */

enum  {
	ACTIVATED,
	
	LAST_SIGNAL
};

enum  {
	GTK_HOTKEY_LISTENER_DUMMY_PROPERTY
};

guint						listener_signals[LAST_SIGNAL] = { 0 };

static  gpointer			gtk_hotkey_listener_parent_class = NULL;

static  GtkHotkeyListener	*default_listener = NULL;
static  GType				default_listener_type = G_TYPE_INVALID;

/**
 * SECTION:gtk-hotkey-listener
 * @short_description: Abstract base class providing platform independent hotkey listening capabilities
 * @see_also: #GtkHotkeyRegistry, #GtkHotkeyInfo
 *
 * #GtkHotkeyListener is an abstract base class for implementing platform
 * specific hotkey listeners - ie objects able to register when the user enters
 * a certain keyboard combination, regardless of which window has focus.
 *
 * Unless you have very special needs you should use the factory method
 * gtk_hotkey_listener_get_default() to get a reference to a #GtkHotkeyListener
 * matching your platform. Although most applications will not need.
 *
 * This class is part of the advanced API of GtkHotkey. Applications will not
 * normally use a #GtkHotkeyListener directly, since gtk_hotkey_info_bind()
 * will call into gtk_hotkey_listener_bind() on the default listener for you.
 **/											

/**
 * gtk_hotkey_listener_get_default
 * @returns: A new reference to the default hotkey listener for the platform
 *
 * Static factory method to get a reference to the default #GtkHotkeyListener
 * for the current platform.
 *
 * FIXME: Currently hardcoded to X11
 */
GtkHotkeyListener*
gtk_hotkey_listener_get_default ()
{
	/* FIXME: This method should be changedd to use the same approach as
	 * gtk_hotkey_registry_get_default() */
	
	if (default_listener) {
		g_return_val_if_fail (GTK_HOTKEY_IS_LISTENER(default_listener), NULL);
		return g_object_ref (default_listener);
	}
	gtk_hotkey_listener_get_type (); /* This call makes sure the default type ise set */
	g_debug ("Listener Type: %s", g_type_name (default_listener_type));
	
	default_listener = g_object_new (default_listener_type, NULL);
	g_return_val_if_fail (GTK_HOTKEY_IS_LISTENER(default_listener), NULL);
	
	return g_object_ref (default_listener);
}

/**
 * gtk_hotkey_listener_bind_hotkey
 * @self: The hotkey listener on which to bind a hotkey
 * @hotkey: The #GtkHotkeyInfo to bind. See #GtkHotkeyInfo:signature
 * @error: #GError in which to store errors, or %NULL to ignore
 * @returns: %TRUE if the binding succeeded, or %FALSE otherwise. In case of
 *           runtime errors @error will be set
 *
 * This method must be implemented by any child class of #GtkHotkeyListener.
 *
 * Start listening for keypresses matching the signature of @hotkey.
 * This method is notmally accessed indirectly by calling gtk_hotkey_info_bind().
 */
gboolean
gtk_hotkey_listener_bind_hotkey (GtkHotkeyListener  *self,
								 GtkHotkeyInfo		*hotkey,
								 GError				**error)
{
	g_return_val_if_fail (GTK_HOTKEY_IS_LISTENER(self), FALSE);
	
	return GTK_HOTKEY_LISTENER_GET_CLASS (self)->bind_hotkey (self, hotkey, error);
}

/**
 * gtk_hotkey_listener_unbind_hotkey
 * @self: The hotkey listener on which to bind a hotkey
 * @hotkey: The #GtkHotkeyInfo to bind. See #GtkHotkeyInfo:signature
 * @error: #GError in which to store errors, or %NULL to ignore
 * @returns: %TRUE if the binding has been removed, or %FALSE otherwise.
 *           In case of runtime errors @error will be set
 *
 * This method must be implemented by any child class of #GtkHotkeyListener.
 *
 * Stop listening for keypresses matching the signature of @hotkey.  This method
 * is notmally accessed indirectly by calling gtk_hotkey_info_unbind().
 */
gboolean
gtk_hotkey_listener_unbind_hotkey (GtkHotkeyListener	*self,
								   GtkHotkeyInfo		*hotkey,
								   GError				**error)
{
	g_return_val_if_fail (GTK_HOTKEY_IS_LISTENER(self), FALSE);
	
	return GTK_HOTKEY_LISTENER_GET_CLASS (self)->unbind_hotkey (self, hotkey, error);
}

/**
 * gtk_hotkey_listener_activated
 * @self: #GtkHotkeyListener to emit the #GtkHotkeyListener::activated signal
 * @hotkey: The #GtkHotkeyInfo the event happened for
 * @event_time: The system time the event happened on. This is useful for
 *              applications to pass through focus stealing prevention when
 *              mapping windows
 *
 * Emit the #GtkHotkeyInfo::activated signal on a hotkey listener.
 */
void
gtk_hotkey_listener_activated (GtkHotkeyListener	*self,
							   GtkHotkeyInfo		*hotkey,
							   guint				event_time)
{
	g_return_if_fail (GTK_HOTKEY_IS_LISTENER(self));
	g_return_if_fail (GTK_HOTKEY_IS_INFO(hotkey));
	
	g_signal_emit (self, listener_signals[ACTIVATED], 0, hotkey, event_time);
}

static void
gtk_hotkey_listener_class_init (GtkHotkeyListenerClass * klass)
{
	gtk_hotkey_listener_parent_class = g_type_class_peek_parent (klass);
	
	/**
	 * GtkHotkeyListener::activated
	 * @listener: The object that emitted the signal
	 * @hotkey: a #GtkHotkeyInfo for the hotkey that was activated
	 * @event_time: Time for event triggering the keypress. This is mainly
	 *              used to pass to window management functions to pass through
	 *              focus stealing prevention
	 *
	 * Emitted when a registered hotkey has been activated.
	 */
	listener_signals[ACTIVATED] = \
	g_signal_new ("activated",
				  GTK_HOTKEY_TYPE_LISTENER,
				  G_SIGNAL_RUN_LAST,
				  0, NULL, NULL,
				  gtk_hotkey_marshal_VOID__OBJECT_UINT,
				  G_TYPE_NONE, 2,
				  GTK_HOTKEY_TYPE_INFO,
				  G_TYPE_UINT);
}


static void
gtk_hotkey_listener_init (GtkHotkeyListener * self)
{
}


GType
gtk_hotkey_listener_get_type (void)
{
	static GType gtk_hotkey_listener_type_id = 0;
	
	if (G_UNLIKELY (gtk_hotkey_listener_type_id == 0)) {
		static const GTypeInfo g_define_type_info = {
			sizeof (GtkHotkeyListenerClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) gtk_hotkey_listener_class_init,
			(GClassFinalizeFunc) NULL,
			NULL,
			sizeof (GtkHotkeyListener),
			0,
			(GInstanceInitFunc) gtk_hotkey_listener_init
		};
		
		gtk_hotkey_listener_type_id = g_type_register_static (G_TYPE_OBJECT,
															  "GtkHotkeyListener",
															  &g_define_type_info,
															  G_TYPE_FLAG_ABSTRACT);
		
		default_listener_type = gtk_hotkey_x11_listener_get_type ();
	}
	return gtk_hotkey_listener_type_id;
}




