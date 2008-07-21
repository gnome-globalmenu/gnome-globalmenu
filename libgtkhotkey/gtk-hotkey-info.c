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

#include "gtk-hotkey-info.h"
#include "gtk-hotkey-error.h"
#include "gtk-hotkey-listener.h"

struct _GtkHotkeyInfoPrivate {
	gchar		*app_id;
	gchar		*key_id;
	GAppInfo	*app_info;
	gchar		*signature;
	gchar		*description;
	GtkHotkeyListener	*listener;
};
#define GTK_HOTKEY_INFO_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTK_HOTKEY_TYPE_INFO, GtkHotkeyInfoPrivate))

enum  {
	GTK_HOTKEY_INFO_DUMMY_PROPERTY,
	GTK_HOTKEY_INFO_BOUND,
	GTK_HOTKEY_INFO_APPLICATION_ID,
	GTK_HOTKEY_INFO_KEY_ID,
	GTK_HOTKEY_INFO_APP_INFO,
	GTK_HOTKEY_INFO_SIGNATURE,
	GTK_HOTKEY_INFO_DESCRIPTION,
};

enum {
	ACTIVATED,
	
	LAST_SIGNAL
};

guint				info_signals[LAST_SIGNAL] = { 0 };

static gpointer		gtk_hotkey_info_parent_class = NULL;

static void			gtk_hotkey_info_finalize (GObject * obj);

/**
 * SECTION:gtk-hotkey-info
 * @short_description: Primary representation of a hotkey
 * @see_also: #GtkHotkeyRegistry
 *
 * #GtkHotkeyInfo is the primary object around which the GtkHotkey library
 * revolves.
 *
 * Hotkeys are stored and managed via a #GtkHotkeyRegistry, while the actual
 * binding and listening for key-presses is done by a #GtkHotkeyListener.
 **/


/**
 * gtk_hotkey_info_bind:
 * @self: The hotkey to bind
 * @error: Place to return a #GError, or %NULL to ignore
 *
 * Register the hotkey with the system. The #GtkHotkeyInfo::activated signal
 * will now be emitted when the user presses the keyboard combination
 * matching the hotkey's signature.
 *
 * Returns: %TRUE on success, and %FALSE on error in which case @error
 *          is also set
 **/
gboolean
gtk_hotkey_info_bind (GtkHotkeyInfo* self, GError **error)
{
	gboolean result;
	
	g_return_val_if_fail (GTK_HOTKEY_IS_INFO (self), FALSE);
	
	if (gtk_hotkey_info_is_bound(self)) {
		g_set_error (error, GTK_HOTKEY_LISTENER_ERROR,
					 GTK_HOTKEY_LISTENER_ERROR_BIND,
					 "Can not bind hotkey '%s' with signature '%s'. "
					 "It is already bound",
					 gtk_hotkey_info_get_key_id(self),
					 gtk_hotkey_info_get_signature(self));
		return FALSE;
	}
	
	if (!self->priv->listener)
		self->priv->listener = gtk_hotkey_listener_get_default ();
	
	g_return_val_if_fail (GTK_HOTKEY_IS_LISTENER(self->priv->listener), FALSE);
	
	result = gtk_hotkey_listener_bind_hotkey (self->priv->listener, self, error);
	if (!result) {
		g_object_unref (self->priv->listener);
		self->priv->listener = NULL;
	}
	
	if (result)
		g_object_notify (G_OBJECT(self), "bound");
	
	return result;
}

/**
 * gtk_hotkey_info_unbind
 * @self: The hotkey to unbind
 * @error: Place to return a #GError, or %NULL to ignore
 * @returns: %TRUE on success, and %FALSE on error in which case @error
 *          is also set
 *
 * Remove the hotkey binding from the system. The #GtkHotkeyInfo::activated
 * signal will no longer be emitted when the hotkey is pressed.
 */
gboolean
gtk_hotkey_info_unbind (GtkHotkeyInfo* self, GError **error)
{	
	gboolean result;
	
	g_return_val_if_fail (GTK_HOTKEY_IS_INFO (self), FALSE);
	
	if (!gtk_hotkey_info_is_bound(self)) {
		g_set_error (error, GTK_HOTKEY_LISTENER_ERROR,
					 GTK_HOTKEY_LISTENER_ERROR_UNBIND,
					 "Can not unbind hotkey '%s' with signature '%s'."
					 "It is not bound",
					 gtk_hotkey_info_get_key_id(self),
					 gtk_hotkey_info_get_signature(self));
		return FALSE;
	}
	
	g_return_val_if_fail (GTK_HOTKEY_IS_LISTENER(self->priv->listener), FALSE);
	
	result = gtk_hotkey_listener_unbind_hotkey (self->priv->listener, self,
												error);
	
	g_object_unref (self->priv->listener);
	self->priv->listener = NULL;
	
	if (result)
		g_object_notify (G_OBJECT(self), "bound");
	
	return result;
}

/**
 * gtk_hotkey_info_is_bound
 * @self: The hotkey to inspect
 * @returns: %TRUE if gtk_hotkey_info_bind() has been called and returned %TRUE
 *           on this hotkey
 *
 * Check whether the hotkey has been succesfully bound to a #GtkHotkeyListener.
 */
gboolean
gtk_hotkey_info_is_bound (GtkHotkeyInfo* self)
{	
	return (self->priv->listener != NULL);
}

/**
 * gtk_hotkey_info_get_application_id
 * @self:
 *
 * Get the unique system identifier for the hotkey. See 
 * #GtkHotkeyInfo:application-id for details.
 */
const gchar*
gtk_hotkey_info_get_application_id (GtkHotkeyInfo* self)
{
	g_return_val_if_fail (GTK_HOTKEY_IS_INFO (self), NULL);
	return self->priv->app_id;
}

/**
 * gtk_hotkey_info_get_key_id
 * @self:
 *
 * Get the identifier the owning application use to identify this hotkey.
 * See #GtkHotkeyInfo:key-id for details.
 */
const gchar*
gtk_hotkey_info_get_key_id (GtkHotkeyInfo* self)
{
	g_return_val_if_fail (GTK_HOTKEY_IS_INFO (self), NULL);
	return self->priv->key_id;
}

/**
 * gtk_hotkey_info_get_app_info
 * @self:
 *
 * Not to be confused with the value of the #GtkHotkeyInfo:application-id
 * property. The hotkey can be associated with an installed desktop application
 * via a #GAppInfo. This is not mandatory and this method returns %NULL
 * if no desktop application has been associated with this hotkey.
 *
 * See the #GtkHotkeyInfo:app-info property for details.
 */
GAppInfo*
gtk_hotkey_info_get_app_info (GtkHotkeyInfo* self)
{
	g_return_val_if_fail (GTK_HOTKEY_IS_INFO (self), NULL);
	return self->priv->app_info;
}

/**
 * gtk_hotkey_info_get_signature
 * @self:
 *
 * Get the keyboard signature of the hotkey. This could for example be
 * '&lt;Alt&gt;F3' or '&lt;Control&gt;&lt;Shift&gt;G'.
 */
const gchar*
gtk_hotkey_info_get_signature (GtkHotkeyInfo* self)
{
	g_return_val_if_fail (GTK_HOTKEY_IS_INFO (self), NULL);
	return self->priv->signature;
}

/**
 * gtk_hotkey_info_get_description
 * @self:
 * @returns: The description of the hotkey or %NULL if none is set
 *
 * Get the free form description of the hotkey. The description is not guaranteed
 * to be set and may be %NULL.
 *
 * FIXME: Do we need to take i18n into account here?
 */
const gchar*
gtk_hotkey_info_get_description (GtkHotkeyInfo* self)
{
	g_return_val_if_fail (GTK_HOTKEY_IS_INFO(self), NULL);
	return self->priv->description;
}

/**
 * gtk_hotkey_info_set_description
 * @self:
 *
 * Set a description for the hotkey. See also gtk_hotkey_info_get_description().
 */
void
gtk_hotkey_info_set_description (GtkHotkeyInfo* self, const gchar *description)
{
	g_return_if_fail (GTK_HOTKEY_IS_INFO(self));
	g_object_set (self, "description", description, NULL); 
}

/**
 * gtk_hotkey_info_equals
 * @hotkey1: The first hotkey to compare
 * @hotkey2: Second hotkey to compare to
 * @sloppy_equals: If %TRUE sloppy equality will be used. This ignores
 *                 the #GtkHotkeyInfo:description and #GtkHotkeyInfo:app-info
 *                 properties of the objects.
 * @returns: %TRUE if all the properties of the hotkeys match. Two %NULL hotkeys
 *           are also considered equal.
 *
 * Compare two #GtkHotkeyInfo<!-- -->s to see if they are equal. This method
 * allows an optional 'sloppy equality' which ignores #GtkHotkeyInfo:description
 * and #GtkHotkeyInfo:app-info.
 */
gboolean
gtk_hotkey_info_equals (GtkHotkeyInfo *hotkey1,
						GtkHotkeyInfo *hotkey2,
						gboolean 			sloppy_equals)
{	
	if (hotkey1 == hotkey2) return TRUE;
	
	g_return_val_if_fail (GTK_HOTKEY_IS_INFO (hotkey1), FALSE);
	g_return_val_if_fail (GTK_HOTKEY_IS_INFO (hotkey2), FALSE);
	
	if (!g_str_equal (gtk_hotkey_info_get_application_id (hotkey1),
					  gtk_hotkey_info_get_application_id (hotkey2)))
		return FALSE;
	
	if (!g_str_equal (gtk_hotkey_info_get_key_id (hotkey1),
					  gtk_hotkey_info_get_key_id (hotkey2)))
		return FALSE;
	
	if (!g_str_equal (gtk_hotkey_info_get_signature (hotkey1),
					  gtk_hotkey_info_get_signature (hotkey2)))
		return FALSE;
	
	/* For sloppy equality this is good enough */
	if (sloppy_equals)
		return TRUE;
	
	const gchar	*d1, *d2;
	d1 = gtk_hotkey_info_get_description (hotkey1);
	d2 = gtk_hotkey_info_get_description (hotkey2);
	if (d1 != NULL && d2 != NULL) {
		if (!g_str_equal (gtk_hotkey_info_get_description (hotkey1),
						  gtk_hotkey_info_get_description (hotkey2)))
			return FALSE;
	} else if (d1 != d2)
		return FALSE;
	/* The case d1 == d2 == NULL will pass through the above */
	
	GAppInfo	*app1, *app2;
	app1 = gtk_hotkey_info_get_app_info (hotkey1);
	app2 = gtk_hotkey_info_get_app_info (hotkey2);
	if (app1 != NULL && app2 != NULL) {
		if (!g_app_info_equal (app1, app2))
			return FALSE;
	} else if (app1 != app2)
		return FALSE;
	/* As above, if app1 == app2 == NULL we count equality */
	
	return TRUE;
}

/**
 * gtk_hotkey_info_activated
 * @self: #GtkHotkeyInfo to emit the #GtkHotkeyInfo::activated signal
 * @event_time: The system time the event happened on. This is useful for
 *              applications to pass through focus stealing prevention when
 *              mapping windows
 *
 * Emit the #GtkHotkeyInfo::activated signal on a hotkey. Mainly called
 * by #GtkHotkeyListener implementations. This method should not normally be
 * used by applications.
 */
void
gtk_hotkey_info_activated (GtkHotkeyInfo* self, guint event_time)
{
	g_return_if_fail (GTK_HOTKEY_IS_INFO(self));
	
	g_signal_emit (self, info_signals[ACTIVATED], 0, event_time);
}

/**
 * gtk_hotkey_info_new:
 * @app_id: Unique identifier the running application can choose for it self. 
 *          May be a free form string, but a descriptive name is encouraged
 * @key_id: A key the application uses to recognize the hotkey. May be a free 
 *          form string, but a descriptive name is encouraged
 * @signature: A key press signature parsable by gtk_accelerator_parse(). For 
 *             examplpe '&lt;Alt&gt;F3' or '&lt;Control&gt;&lt;Shift&gt;G'.
 * @app_info: An optional #GAppInfo to associate with the hotkey. Pass %NULL to
 *            ignore this
 * @returns: A new #GtkHotkeyInfo or %NULL on error. Error conditions could for 
 *           example be invalid an invalid @signature, or %NULL arguments.
 *
 * Create a new hotkey. To actually trigger the hotkey when the user enters
 * the right keyboard combination call gtk_hotkey_info_bind(). To save and
 * load your hotkey settings use the #GtkHotkeyRegistry provided by
 * gtk_hotkey_registry_get_default().
 **/
GtkHotkeyInfo*
gtk_hotkey_info_new (const gchar	*app_id,
					 const gchar	*key_id,
					 const gchar	*signature,
					 GAppInfo		*app_info)
{
	GtkHotkeyInfo * self;
	
	g_return_val_if_fail (app_id != NULL, NULL);
	g_return_val_if_fail (key_id != NULL, NULL);
	
	/* A NULL app_info is ok, but it better be a GAppInfo then */
	if (app_info != NULL)
		g_return_val_if_fail (G_IS_APP_INFO(app_info), NULL);
	
	self = g_object_new (GTK_HOTKEY_TYPE_INFO, "application-id", app_id,
											   "key-id", key_id,
											   "signature", signature,
											   "app-info", app_info,
												NULL);
	return self;
}

static void
gtk_hotkey_info_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec)
{
	GtkHotkeyInfo * self;
	
	self = GTK_HOTKEY_INFO (object);
	
	switch (property_id) {
		case GTK_HOTKEY_INFO_BOUND:
			g_value_set_boolean (value,
								 (self->priv->listener != NULL));
			break;
		case GTK_HOTKEY_INFO_APPLICATION_ID:
			g_value_set_string (value,
								gtk_hotkey_info_get_application_id (self));
			break;
		case GTK_HOTKEY_INFO_KEY_ID:
			g_value_set_string (value,
								gtk_hotkey_info_get_key_id (self));
			break;
		case GTK_HOTKEY_INFO_APP_INFO:
			g_value_set_object (value,
								gtk_hotkey_info_get_app_info (self));
			break;
		case GTK_HOTKEY_INFO_SIGNATURE:
			g_value_set_string (value,
								gtk_hotkey_info_get_signature (self));
			break;
		case GTK_HOTKEY_INFO_DESCRIPTION:
			g_value_set_string (value,
								gtk_hotkey_info_get_description (self));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}


static void
gtk_hotkey_info_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec)
{
	GtkHotkeyInfo			*self;
	GtkHotkeyInfoPrivate	*priv;
	
	self = GTK_HOTKEY_INFO (object);
	priv = self->priv;
	
	switch (property_id) {
		case GTK_HOTKEY_INFO_BOUND:
			g_critical ("Writing to read only property 'bound'");
			break;
		case GTK_HOTKEY_INFO_APPLICATION_ID:
			if (priv->app_id)
				g_critical ("Overwriting construct only property 'application-id'");
			priv->app_id = g_value_dup_string (value);
			break;
		case GTK_HOTKEY_INFO_KEY_ID:
			if (priv->key_id)
				g_critical ("Overwriting construct only property 'key-id'");
			priv->key_id = g_value_dup_string (value);
			break;
		case GTK_HOTKEY_INFO_APP_INFO:
			if (priv->app_info)
				g_critical ("Overwriting construct only property 'app-info'");
			priv->app_info = g_value_dup_object (value);
			break;
		case GTK_HOTKEY_INFO_SIGNATURE:
			if (priv->signature)
				g_critical ("Overwriting construct only property 'signature'");
			priv->signature = g_value_dup_string (value);
			break;
		case GTK_HOTKEY_INFO_DESCRIPTION:
			if (priv->description)
				g_free(priv->description);
			priv->description = g_value_dup_string (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}


static void
gtk_hotkey_info_class_init (GtkHotkeyInfoClass * klass)
{
	gtk_hotkey_info_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (GtkHotkeyInfoPrivate));
	
	G_OBJECT_CLASS (klass)->get_property = gtk_hotkey_info_get_property;
	G_OBJECT_CLASS (klass)->set_property = gtk_hotkey_info_set_property;
	G_OBJECT_CLASS (klass)->finalize = gtk_hotkey_info_finalize;
	
	/**
	 * GtkHotkeyInfo:bound
	 *
	 * Property reflecting whether or not this hotkey has been bound to
	 * a #GtkHotkeyListener. If this property is %TRUE you will receive
	 * #GtkHotkeyInfo::activated signals when the hotkey is triggered
	 * by the user.
	 */
	g_object_class_install_property (G_OBJECT_CLASS (klass),
									 GTK_HOTKEY_INFO_BOUND,
									 g_param_spec_boolean ("bound",
														  "Is Bound",
														  "Whether or not the hotkey is bound to a GtkHotkeyListener",
														  FALSE, 
														  G_PARAM_READABLE));
	
	/**
	 * GtkHotkeyInfo:application-id
	 *
	 * A free form string chosen by the application using the hotkey, under
	 * which the application identifies itself.
	 */
	g_object_class_install_property (G_OBJECT_CLASS (klass),
									 GTK_HOTKEY_INFO_APPLICATION_ID,
									 g_param_spec_string ("application-id",
														  "Application Id",
														  "Globally unique application id",
														  NULL, 
														  G_PARAM_READABLE | G_PARAM_WRITABLE |
														  G_PARAM_CONSTRUCT_ONLY));
	
	/**
	 * GtkHotkeyInfo:key-id
	 *
	 * A free form string the application using the hotkey has attributed
	 * the hotkey so that it can be identified later on. Applications are
	 * encouraged to choose descriptive key ids.
	 */
	g_object_class_install_property (G_OBJECT_CLASS (klass),
									 GTK_HOTKEY_INFO_KEY_ID,
									 g_param_spec_string ("key-id",
														  "Hotkey Id",
														  "Globally unique identifier for the hotkey",
														  NULL, 
														  G_PARAM_READABLE | G_PARAM_WRITABLE |
														  G_PARAM_CONSTRUCT_ONLY));
	
	/**
	 * GtkHotkeyInfo:app-info
	 *
	 * A #GAppInfo associated with the key. This is mainly useful for external
	 * applications which can use the information provided by the #GAppInfo
	 * to display meaningful messages to the user. Like 'The keyboard 
	 * combination &lt;Alt&gt;F3' is already assigned to the application
	 * "Deskbar Applet", please select another'.
	 */
	g_object_class_install_property (G_OBJECT_CLASS (klass),
									 GTK_HOTKEY_INFO_APP_INFO,
									 g_param_spec_object ("app-info",
														  "Application Information",
														  "Object holding metadata about "
														  "the hotkey's application",
														  G_TYPE_APP_INFO, 
														  G_PARAM_READABLE | G_PARAM_WRITABLE |
														  G_PARAM_CONSTRUCT_ONLY));
	
	/**
	 * GtkHotkeyInfo:signature
	 *
	 * The keyboard signature of the hotkey. This could for example by
	 * '&lt;Alt&gt;F3' or '&lt;Control&gt;&lt;Shift&gt;G'. The signature should be parsable by
	 * gtk_accelerator_parse().
	 */
	g_object_class_install_property (G_OBJECT_CLASS (klass),
									 GTK_HOTKEY_INFO_SIGNATURE,
									 g_param_spec_string ("signature",
														  "Signature",
														  "String defining the keyboard shortcut",
														  NULL, 
														  G_PARAM_READABLE | G_PARAM_WRITABLE |
														  G_PARAM_CONSTRUCT_ONLY));
	
	/**
	 * GtkHotkeyInfo:description
	 *
	 * An optional free form description of the hotkey.
	 */
	g_object_class_install_property (G_OBJECT_CLASS (klass),
									 GTK_HOTKEY_INFO_DESCRIPTION,
									 g_param_spec_string ("description",
														  "Description",
														  "Short description of what happens upon activation",
														  "", 
														  G_PARAM_READABLE | G_PARAM_WRITABLE));
	
	/**
	 * GtkHotkeyInfo::activated:
	 * @hotkey: a #GtkHotkeyInfo for the hotkey that was activated
	 * @event_time: Time for event triggering the keypress. This is mainly
	 *              used to pass to window management functions to pass through
	 *              focus stealing prevention
	 *
	 * Emitted when a hotkey has been activated.
	 */
	info_signals[ACTIVATED] = \
	g_signal_new ("activated",
				  GTK_HOTKEY_TYPE_INFO,
				  G_SIGNAL_RUN_LAST,
				  0, NULL, NULL,
				  g_cclosure_marshal_VOID__UINT,
				  G_TYPE_NONE, 1,
				  G_TYPE_UINT);
}


static void
gtk_hotkey_info_init (GtkHotkeyInfo * self)
{
	self->priv = GTK_HOTKEY_INFO_GET_PRIVATE (self);
	
	self->priv->app_id = NULL;
	self->priv->key_id = NULL;
	self->priv->app_info = NULL;
}


static void
gtk_hotkey_info_finalize (GObject * obj)
{
	GtkHotkeyInfo			*self;
	GtkHotkeyInfoPrivate	*priv;
	
	self = GTK_HOTKEY_INFO (obj);
	priv = self->priv;
	
	if (priv->app_id)
		g_free (priv->app_id);
	if (priv->key_id)
		g_free (priv->key_id);
	if (priv->app_info)
		g_object_unref (priv->app_info);
	if (priv->signature)
		g_free (priv->signature);
	if (priv->description)
		g_free (priv->description);
	if (GTK_HOTKEY_IS_LISTENER (priv->listener))
		g_object_unref (priv->listener);
	
	G_OBJECT_CLASS (gtk_hotkey_info_parent_class)->finalize (obj);
}


GType
gtk_hotkey_info_get_type (void)
{
	static GType gtk_hotkey_info_type_id = 0;
	
	if (G_UNLIKELY (gtk_hotkey_info_type_id == 0)) {
		static const GTypeInfo g_define_type_info = {
			sizeof (GtkHotkeyInfoClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) gtk_hotkey_info_class_init,
			(GClassFinalizeFunc) NULL,
			NULL,
			sizeof (GtkHotkeyInfo),
			0,
			(GInstanceInitFunc) gtk_hotkey_info_init
		};
		
		gtk_hotkey_info_type_id = g_type_register_static (G_TYPE_OBJECT,
														  "GtkHotkeyInfo",
														  &g_define_type_info,
														  0);
	}
	
	return gtk_hotkey_info_type_id;
}




