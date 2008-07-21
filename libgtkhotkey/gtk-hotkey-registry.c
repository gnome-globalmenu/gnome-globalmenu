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
 
#include "gtk-hotkey-registry.h"
#include "gtk-hotkey-key-file-registry.h"

enum  {
	GTK_HOTKEY_REGISTRY_DUMMY_PROPERTY
};

enum {
	HOTKEY_STORED,
	HOTKEY_DELETED,
	
	LAST_SIGNAL
};

static gpointer		gtk_hotkey_registry_parent_class = NULL;

static GType		default_registry_type = G_TYPE_INVALID;

static GtkHotkeyRegistry
					*default_registry = NULL;

#define DEFAULT_REGISTRY_TYPE GTK_HOTKEY_TYPE_KEY_FILE_REGISTRY

guint				storage_signals[LAST_SIGNAL] = { 0 };

/**
 * SECTION:gtk-hotkey-registry
 * @short_description: Abstract base class for services storing and loading hotkeys
 * @see_also: #GtkHotkeyKeyFileRegistry
 *
 * #GtkHotkeyRegistry is an abstract base class for implementing a platform
 * specific service for storing and loading hotkey configurations.
 *
 * The actual binding of the hotkey into the environment is done by a
 * #GtkHotkeyListener. This class is only meant to do the management part of the
 * hotkey handling.
 *
 * The reasong why applications should use a #GtkHotkeyRegistry and not just a
 * flat text file with the hotkey signature is to make sure we don't overwrite
 * or interfere with the hotkeys of other applications. And possibly providing
 * a unified user interface for managing the hotkeys of all applications.
 *
 * To obtain a #GtkHotkeyRegistry matching your desktop environment use
 * the factory method gtk_hotkey_registry_get_default().
 *
 **/	

/**
 * gtk_hotkey_registry_get_default
 * @returns: A reference to a #GtkHotkeyRegistry matching your platform
 *
 * Currently the only implementation of this class is #GtkHotkeyKeyFileRegistry.
 */
GtkHotkeyRegistry*
gtk_hotkey_registry_get_default (void)
{	
	if (G_UNLIKELY(default_registry == NULL)) {
		
		/* Set the default type of registry to create */
		if (default_registry_type == G_TYPE_INVALID)
			default_registry_type = DEFAULT_REGISTRY_TYPE;
		
		default_registry = GTK_HOTKEY_REGISTRY (g_object_new (GTK_HOTKEY_TYPE_KEY_FILE_REGISTRY,
															NULL));
		g_return_val_if_fail (GTK_HOTKEY_IS_REGISTRY(default_registry), NULL);
		/* We always keep a ref to the registry here */
	}
	return g_object_ref(default_registry);
}

/**
 * gtk_hotkey_registry_get_hotkey
 * @self: The registry to search in
 * @app_id: The name under which the application has registered it self
 *          when it created the #GtkHotkeyInfo in the first place.
 * @key_id: The id assignedd to the actual hotkey on the moment of its creation
 * @error: Place to store a #GError in case of errors, or %NULL to ignore
 * @returns: The #GtkHotkeyInfo for the requested parameters or %NULL is none
 *           where found. In the case %NULL is returned @error will be set
 *           accordingly. Free the hotkey with g_object_unref() when you are done
 *           using it.
 *
 * Look up a hotkey given its id and application id.
 */
GtkHotkeyInfo*
gtk_hotkey_registry_get_hotkey (GtkHotkeyRegistry	 *self,
							   const char		*app_id,
							   const char		*key_id,
							   GError			**error)
{
	g_return_val_if_fail (GTK_HOTKEY_IS_REGISTRY(self), NULL);
	return GTK_HOTKEY_REGISTRY_GET_CLASS (self)->get_hotkey (self, app_id, key_id,
															error);
}

/**
 * gtk_hotkey_registry_get_application_hotkeys
 * @self: The #GtkHotkeyRegistry to look hotkeys up in
 * @app_id: Unique application id
 * @error: Place to return a #GError or %NULL
 * @returns: A list of #GtkHotkeyInfo objects. The list should be with freed with
 *           g_list_free() and the hotkey objects should be freed with
 *           g_object_unref().
 * 
 * Look up all hotkeys registered by a given application.
 */
GList*
gtk_hotkey_registry_get_application_hotkeys (GtkHotkeyRegistry	*self,
											const char			*app_id,
											GError				**error)
{
	g_return_val_if_fail (GTK_HOTKEY_IS_REGISTRY(self), NULL);
	return GTK_HOTKEY_REGISTRY_GET_CLASS (self)->get_application_hotkeys (self, app_id, error);
}

/**
 * gtk_hotkey_registry_get_all_hotkeys
 * @self: The #GtkHotkeyRegistry to look hotkeys up in
 * @returns: A list of all valid #GtkHotkeyInfo<!-- -->s stored in the registry. 
 *           The list should be with freed with g_list_free() and the hotkey 
 *           objects should be freed with g_object_unref().
 * 
 * Look up all hotkeys registered by a given application.
 */
GList*
gtk_hotkey_registry_get_all_hotkeys (GtkHotkeyRegistry	*self)
{
	g_return_val_if_fail (GTK_HOTKEY_IS_REGISTRY(self), NULL);
	return GTK_HOTKEY_REGISTRY_GET_CLASS (self)->get_all_hotkeys (self);
}

/**
 * gtk_hotkey_registry_store_hotkey
 * @self: The #GtkHotkeyRegistry in which to store the hotkey
 * @info: The #GtkHotkeyInfo to store
 * @error: Place to return a #GError or %NULL to ignore
 * @returns: %TRUE on success and %FALS otherwise. In case of errors @error
 *           will be set accordingly.
 *           
 * Store a hotkey in the registry for later usage. In case of success the
 * #GtkHotkeyRegistry::hotkey-stored signal will be emitted.
 */
gboolean
gtk_hotkey_registry_store_hotkey (GtkHotkeyRegistry   *self,
								 GtkHotkeyInfo		*info,
								 GError				**error)
{
	g_return_val_if_fail (GTK_HOTKEY_IS_REGISTRY(self), FALSE);
	return GTK_HOTKEY_REGISTRY_GET_CLASS (self)->store_hotkey (self, info, error);
}

/**
 * gtk_hotkey_registry_delete_hotkey
 * @self: The #GtkHotkeyRegistry from which to delete the hotkey
 * @app_id: The value of the #GtkHotkeyInfo:application-id property of the stored
 *          hotkey
 * @key_id: The value of the #GtkHotkeyInfo:key-id property of the stored hotkey
 * @error: Place to return a #GError or %NULL to ignore
 * @returns: %TRUE on success and %FALS otherwise. In case of errors @error
 *           will be set accordingly.
 *           
 * Delete a hotkey from the registry. In case of success the
 * #GtkHotkeyRegistry::hotkey-deleted signal will be emitted.
 */
gboolean
gtk_hotkey_registry_delete_hotkey (GtkHotkeyRegistry	*self,
								  const gchar		*app_id,
								  const gchar		*key_id,
								  GError			**error)
{
	g_return_val_if_fail (GTK_HOTKEY_IS_REGISTRY(self), FALSE);
	return GTK_HOTKEY_REGISTRY_GET_CLASS (self)->delete_hotkey (self, app_id,
															   key_id, error);
}

/**
 * gtk_hotkey_registry_has_hotkey
 * @self: The #GtkHotkeyRegistry to look hotkeys up in
 * @app_id: The value of the #GtkHotkeyInfo:application-id property of the stored
 *          hotkey
 * @key_id: The value of the #GtkHotkeyInfo:key-id property of the stored hotkey
 * @returns: %TRUE if the registry has stored a hotkey with with application id
 *           @app_id and hotkey id @key_id.
 * 
 * Look up all hotkeys registered by a given application.
 */
gboolean
gtk_hotkey_registry_has_hotkey (GtkHotkeyRegistry		*self,
							   const gchar			*app_id,
							   const gchar			*key_id)
{
	g_return_val_if_fail (GTK_HOTKEY_IS_REGISTRY(self), FALSE);
	return GTK_HOTKEY_REGISTRY_GET_CLASS (self)->has_hotkey (self, app_id, key_id);
}

/**
 * gtk_hotkey_registry_hotkey_stored
 * @self: The #GtkHotkeyRegistry to emit the signal on
 * @info: The #GtkHotkeyInfo that was stored
 * 
 * Emit the #GtkHotkeyRegistry::hotkey-stored signal on @self. This method should
 * only be used by child classes of #GtkHotkeyRegistry.
 */
void
gtk_hotkey_registry_hotkey_stored (GtkHotkeyRegistry	*self,
								 GtkHotkeyInfo		*info)
{
	g_return_if_fail (GTK_HOTKEY_IS_REGISTRY(self));
	g_return_if_fail (GTK_HOTKEY_IS_INFO(info));
	
	GTK_HOTKEY_REGISTRY_GET_CLASS (self)->hotkey_stored (self, info);
}

/**
 * gtk_hotkey_registry_hotkey_deleted
 * @self: The #GtkHotkeyRegistry to emit the signal on
 * @info: The #GtkHotkeyInfo that was deleted
 * 
 * Emit the #GtkHotkeyRegistry::hotkey-deleted signal on @self. This method should
 * only be used by child classes of #GtkHotkeyRegistry.
 */
void
gtk_hotkey_registry_hotkey_deleted (GtkHotkeyRegistry		*self,
								   GtkHotkeyInfo		*info)
{
	g_return_if_fail (GTK_HOTKEY_IS_REGISTRY(self));
	GTK_HOTKEY_REGISTRY_GET_CLASS (self)->hotkey_deleted (self, info);
}

static void
gtk_hotkey_registry_hotkey_stored_real (GtkHotkeyRegistry	*self,
									   GtkHotkeyInfo	*info)
{
	g_return_if_fail (GTK_HOTKEY_IS_INFO(info));
	g_return_if_fail (GTK_HOTKEY_IS_REGISTRY(self));
	
	g_signal_emit (self, storage_signals[HOTKEY_STORED], 0, info);
}

static void
gtk_hotkey_registry_hotkey_deleted_real (GtkHotkeyRegistry	*self,
										GtkHotkeyInfo		*info)
{
	g_return_if_fail (GTK_HOTKEY_IS_INFO(info));
	g_return_if_fail (GTK_HOTKEY_IS_REGISTRY(self));
	
	g_signal_emit (self, storage_signals[HOTKEY_DELETED], 0, info);
}

static void
gtk_hotkey_registry_class_init (GtkHotkeyRegistryClass *klass)
{
	gtk_hotkey_registry_parent_class = g_type_class_peek_parent (klass);
	
	klass->hotkey_stored = gtk_hotkey_registry_hotkey_stored_real;
	klass->hotkey_deleted = gtk_hotkey_registry_hotkey_deleted_real;
	
	/**
	 * GtkHotkeyRegistry::hotkey-stored
	 * @hotkey:The hotkey that was stored
	 *
	 * Emitted when a hotkey has been stored in the registry
	 */
	storage_signals[HOTKEY_STORED] = \
	g_signal_new ("hotkey_stored",
				  GTK_HOTKEY_TYPE_STORAGE,
				  G_SIGNAL_RUN_LAST,
				  0, NULL, NULL,
				  g_cclosure_marshal_VOID__OBJECT,
				  G_TYPE_NONE, 1,
				  G_TYPE_OBJECT);
	
	/**
	 * GtkHotkeyRegistry::hotkey-deleted
	 * @hotkey:The hotkey that was deleted
	 *
	 * Emitted when a hotkey has been deleted from the registry
	 */
	storage_signals[HOTKEY_DELETED] = \
	g_signal_new ("hotkey_deleted",
				  GTK_HOTKEY_TYPE_STORAGE,
				  G_SIGNAL_RUN_LAST,
				  0, NULL, NULL,
				  g_cclosure_marshal_VOID__OBJECT,
				  G_TYPE_NONE, 1,
				  G_TYPE_OBJECT);
}


static void
gtk_hotkey_registry_init (GtkHotkeyRegistry * self)
{
	
}

static void
gtk_hotkey_registry_finalize (GtkHotkeyRegistry * self)
{
	
}

GType
gtk_hotkey_registry_get_type (void)
{
	static GType gtk_hotkey_registry_type_id = 0;
	
	if (G_UNLIKELY (gtk_hotkey_registry_type_id == 0)) {
		static const GTypeInfo g_define_type_info = {
			sizeof (GtkHotkeyRegistryClass),
			(GBaseInitFunc) gtk_hotkey_registry_init,
			(GBaseFinalizeFunc) gtk_hotkey_registry_finalize,
			(GClassInitFunc) gtk_hotkey_registry_class_init,
			(GClassFinalizeFunc) NULL,
			NULL,
			sizeof (GtkHotkeyRegistry),
			0,
			(GInstanceInitFunc) gtk_hotkey_registry_init 
		};
		
		gtk_hotkey_registry_type_id = g_type_register_static (G_TYPE_OBJECT, "GtkHotkeyRegistry", &g_define_type_info, G_TYPE_FLAG_ABSTRACT);
	}
	return gtk_hotkey_registry_type_id;
}
