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
 
#include "gtk-hotkey-key-file-registry.h"
#include "gtk-hotkey-utils.h"

#include <gio/gio.h>

enum  {
	GTK_HOTKEY_KEY_FILE_REGISTRY_DUMMY_PROPERTY
};

/* Beware - the lengths of these strings are hardcoded throughout
 * this file (sorry) */
#define HOTKEY_HOME "~/.config/hotkeys"
#define HOTKEY_FILE_EXT ".hotkeys"
#define HOTKEY_GROUP "hotkey:"

static GtkHotkeyInfo*	gtk_hotkey_key_file_registry_real_get_hotkey	(GtkHotkeyRegistry	*base,
																	 const char			*app_id,
																	 const char			*key_id,
																	 GError				**error);

static GList*			gtk_hotkey_key_file_registry_real_get_application_hotkeys
																	(GtkHotkeyRegistry	*base,
																	 const char			*app_id,
																	 GError				**error);

static GList*			gtk_hotkey_key_file_registry_real_get_all_hotkeys
																	(GtkHotkeyRegistry	*base);

static gboolean			gtk_hotkey_key_file_registry_real_store_hotkey(GtkHotkeyRegistry	*base,
																	 GtkHotkeyInfo		*info,
																	 GError				**error);

static gboolean			gtk_hotkey_key_file_registry_real_delete_hotkey
																	(GtkHotkeyRegistry	*base,
																	 const gchar		*app_id,
																	 const gchar		*key_id,
																	 GError				**error);

static gboolean			gtk_hotkey_key_file_registry_real_has_hotkey  (GtkHotkeyRegistry	*base,
																	 const gchar		*app_id,
																	 const gchar		*key_id);

static GFile*			get_hotkey_home								(void);

static GFile*			get_hotkey_file								(const gchar		*app_id);

static GKeyFile*		get_hotkey_key_file							(const gchar		*app_id,
																	 GError				**error);

static GtkHotkeyInfo*   get_hotkey_info_from_key_file				(GKeyFile			*keyfile,
																	 const gchar		*app_id,
																	 const gchar		*key_id,
																	 GError				**error);

static GList*			get_all_hotkey_infos_from_key_file			(GKeyFile			*keyfile,
																	 const gchar		*app_id);

static gpointer gtk_hotkey_key_file_registry_parent_class = NULL;


static
GtkHotkeyInfo*
gtk_hotkey_key_file_registry_real_get_hotkey (GtkHotkeyRegistry	*base,
											const char			*app_id,
											const char			*key_id,
											GError				**error)
{
	GtkHotkeyKeyFileRegistry	*self;
	GKeyFile					*keyfile = NULL;
	GtkHotkeyInfo				*info = NULL;
	
	g_return_val_if_fail (GTK_HOTKEY_IS_KEY_FILE_REGISTRY(base), NULL);
	g_return_val_if_fail (app_id != NULL, NULL);
	g_return_val_if_fail (key_id != NULL, NULL);
	
	self = GTK_HOTKEY_KEY_FILE_REGISTRY (base);
	
	keyfile = get_hotkey_key_file (app_id, error);
	if (keyfile == NULL)
		goto clean_up;
	
	info = get_hotkey_info_from_key_file (keyfile, app_id, key_id, error);
	
	clean_up:
		if (keyfile) g_key_file_free (keyfile);
	
	return info;
	
}


static GList*
gtk_hotkey_key_file_registry_real_get_application_hotkeys (GtkHotkeyRegistry	*base,
														 const char			*app_id,
														 GError				**error)
{
	GtkHotkeyKeyFileRegistry		*self;
	GKeyFile					*keyfile;
	
	g_return_val_if_fail (app_id != NULL, NULL);
	
	self = GTK_HOTKEY_KEY_FILE_REGISTRY (base);
	keyfile = get_hotkey_key_file (app_id, error);
	
	if (keyfile == NULL)
		return NULL; /* error is set by get_hotkey_key_file() */
	
	return get_all_hotkey_infos_from_key_file (keyfile, app_id);
}


static GList*
gtk_hotkey_key_file_registry_real_get_all_hotkeys (GtkHotkeyRegistry *base)
{
	GtkHotkeyKeyFileRegistry *self;
	GFile					*home;
	GFileEnumerator			*dir;
	GFileInfo				*file_info;
	GError					*error;
	GList					*result = NULL;
	
	self = GTK_HOTKEY_KEY_FILE_REGISTRY (base);
	home = get_hotkey_home ();
	
	error = NULL;
	dir = g_file_enumerate_children (home, G_FILE_ATTRIBUTE_STANDARD_NAME,
									 0, NULL, &error);
	if (error) {
		gchar *path = g_file_get_path (home);
		g_critical ("Failed to read hotkey home directory '%s': %s",
					path, error->message);
		g_free (path);
		g_error_free (error);
		return NULL;
	}
	
	error = NULL;
	while ((file_info = g_file_enumerator_next_file (dir, NULL, &error)) != NULL) {
		const gchar *filename;
		GFile		*file;
		GString		*app_id;
		GList		*app_hotkeys;
		
		filename = g_file_info_get_name(file_info);
		
		if (g_str_has_suffix (filename, HOTKEY_FILE_EXT)) {
			file = g_file_get_child (home, filename);
			
			/* Extract app_id from file name */
			app_id = g_string_new (filename);
			g_string_erase (app_id, app_id->len - 8, 8);
			
			/* Load all hotkeys from the file, and append it to
			 * the total result */
			app_hotkeys = gtk_hotkey_registry_get_application_hotkeys (base,
																	  app_id->str,
																	  &error);
			if (error) {
				g_warning ("Failed to read hotkeys for application '%s': %s",
						   app_id->str, error->message);
				g_error_free (error);
				error = NULL;
			} else {
				result = g_list_concat (result, app_hotkeys);
			}
			
			g_string_free (app_id, TRUE);
			g_object_unref (file);
		}
		
		g_object_unref (file_info);
	}
	
	if (error) {
		gchar *path = g_file_get_path (home);
		g_warning ("Failed to read hotkey home directory '%s': %s",
				   path, error->message);
		g_free (path);
		g_error_free (error);
	}
	
	
	g_object_unref (dir);
	g_object_unref (home);
	
	return result;
}


static gboolean
gtk_hotkey_key_file_registry_real_store_hotkey (GtkHotkeyRegistry	*base,
											  GtkHotkeyInfo		*info,
											  GError			**error)
{
	GtkHotkeyKeyFileRegistry		*self;
	GKeyFile					*keyfile;
	GFile						*file, *home;
	GError						*tmp_error;
	gchar						*file_path, *group;
	
	
	self = GTK_HOTKEY_KEY_FILE_REGISTRY (base);
	g_return_val_if_fail (GTK_HOTKEY_IS_INFO (info), FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);
	
	/* Make sure we have our root dir */
	home = get_hotkey_home ();
	if (!g_file_query_exists(home, NULL)) {
		tmp_error = NULL;
		if (!g_file_make_directory (home, NULL, &tmp_error)) {
			g_set_error (error, GTK_HOTKEY_REGISTRY_ERROR,
						 GTK_HOTKEY_REGISTRY_ERROR_IO,
						 "Failed to create hotkey configuration dir "
						HOTKEY_HOME": %s", tmp_error->message);
			g_error_free (tmp_error);
			g_object_unref (home);
			return FALSE;
		}
	}
	
	/* Now load any old contents of the keyfile */
	file = get_hotkey_file (gtk_hotkey_info_get_application_id (info));
	file_path = g_file_get_path (file);
	keyfile = g_key_file_new ();
	
	tmp_error = NULL;
	if (!g_key_file_load_from_file (keyfile, file_path, 0, &tmp_error)) {
		if (tmp_error->code == G_KEY_FILE_ERROR_PARSE) {
			g_set_error (error, GTK_HOTKEY_REGISTRY_ERROR,
						 GTK_HOTKEY_REGISTRY_ERROR_MALFORMED_MEDIUM,
						 "The file %s is not in a valid key-file format: %s",
						 file_path, tmp_error->message);
			goto clean_up;
		}
		/* Ignore other errors */
		g_error_free (tmp_error);
	}
	
	/* Prepare keyfile data */
	group = g_strconcat (HOTKEY_GROUP, gtk_hotkey_info_get_key_id (info), NULL);
	
	g_key_file_set_string (keyfile, group, "Owner",
						   gtk_hotkey_info_get_application_id (info));
	g_key_file_set_string (keyfile, group, "Signature",
						   gtk_hotkey_info_get_signature (info));
	
	if (gtk_hotkey_info_get_description (info))
		g_key_file_set_string (keyfile, group, "Description",
							   gtk_hotkey_info_get_description (info));
	
	if (gtk_hotkey_info_get_app_info (info)) {
		GAppInfo *ai = gtk_hotkey_info_get_app_info (info);
		g_key_file_set_string (keyfile, group, "AppInfo",
							   g_app_info_get_id (ai));
	}
	
	gsize size;
	gchar *contents;
	tmp_error = NULL;
	contents = g_key_file_to_data (keyfile, &size, &tmp_error);
	if (tmp_error) {
		g_set_error (error, GTK_HOTKEY_REGISTRY_ERROR,
					 GTK_HOTKEY_REGISTRY_ERROR_UNKNOWN,
					 "Failed to generate keyfile contents: %s",
					 tmp_error->message);
		goto clean_up;
	}
	
	/* Write the actual data */
	g_file_set_contents (file_path, contents, size, &tmp_error);
	if (tmp_error) {
		g_set_error (error, GTK_HOTKEY_REGISTRY_ERROR,
					 GTK_HOTKEY_REGISTRY_ERROR_IO,
					 "Failed to write keyfile '%s': %s",
					 file_path, tmp_error->message);
		goto clean_up;
	}
	
	clean_up:
		if (tmp_error) g_error_free (tmp_error);
		g_free (file_path);
		if (group) g_free (group);
		g_key_file_free (keyfile);
		g_object_unref (file);
		g_object_unref (home);
	
	if (*error)
		return FALSE;
	
			self = GTK_HOTKEY_KEY_FILE_REGISTRY (base);
	g_return_val_if_fail (GTK_HOTKEY_IS_INFO (info), FALSE);
	gtk_hotkey_registry_hotkey_stored (base, info);
	return TRUE;
}

static gboolean
gtk_hotkey_key_file_registry_real_delete_hotkey (GtkHotkeyRegistry	*base,
											   const gchar		*app_id,
											   const gchar		*key_id,
											   GError			**error)
{
	GtkHotkeyKeyFileRegistry *self;
	GtkHotkeyInfo			*info = NULL;
	GFile					*file;
	GKeyFile				*keyfile;
	GError					*tmp_error;
	gboolean				is_error = FALSE;
	gchar					*path, *group;
	
	g_return_val_if_fail (app_id != NULL, FALSE);
	g_return_val_if_fail (key_id != NULL, FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);
	
	self = GTK_HOTKEY_KEY_FILE_REGISTRY (base);
	group = NULL;
	
	file = get_hotkey_file (app_id);
	g_return_val_if_fail (G_IS_FILE(file), FALSE);
	
	path = g_file_get_path (file);
	keyfile = g_key_file_new ();
	
	/* Load the old keyfile */
	tmp_error = NULL;
	g_key_file_load_from_file (keyfile, path, 0, &tmp_error);
	if (tmp_error) {
		if ((tmp_error->domain == G_FILE_ERROR &&
			 tmp_error->code == G_FILE_ERROR_NOENT) ||
			(tmp_error->domain == G_KEY_FILE_ERROR &&
			 tmp_error->code == G_KEY_FILE_ERROR_NOT_FOUND))
			g_set_error (error, GTK_HOTKEY_REGISTRY_ERROR,
						 GTK_HOTKEY_REGISTRY_ERROR_UNKNOWN_APP,
						 "No such keyfile '%s'. Application '%s' has not "
						 "registered any hotkeys",
						 path, app_id);
		else
			g_set_error (error, GTK_HOTKEY_REGISTRY_ERROR,
						 GTK_HOTKEY_REGISTRY_ERROR_IO,
						 "Failed to load keyfile '%s': %s",
						 app_id, tmp_error->message);
		is_error = TRUE;
		goto clean_up;
	}
	
	/* Get a ref to the GtkHotkeyInfo so that we can emit it with the
	 * hotkey-deleted signal */
	tmp_error = NULL;
	info = get_hotkey_info_from_key_file (keyfile, app_id, key_id, error);
	if (info == NULL) {
		is_error = TRUE;
		goto clean_up;
	}
	
	/* Remove the group for key_id */
	group = g_strconcat (HOTKEY_GROUP, key_id, NULL);
	tmp_error = NULL;
	g_key_file_remove_group (keyfile, group, &tmp_error);
	if (tmp_error) {
		if (tmp_error->domain == G_KEY_FILE_ERROR &&
			 tmp_error->code == G_KEY_FILE_ERROR_GROUP_NOT_FOUND)
			g_set_error (error, GTK_HOTKEY_REGISTRY_ERROR,
						 GTK_HOTKEY_REGISTRY_ERROR_UNKNOWN_APP,
						 "Application '%s' has not registered a hotkey with"
						 "id '%s'", app_id, key_id);
		else
			g_set_error (error, GTK_HOTKEY_REGISTRY_ERROR,
						 GTK_HOTKEY_REGISTRY_ERROR_UNKNOWN,
						 "Failed to delete hotkey '%s' from application %s: %s",
						 key_id, app_id, tmp_error->message);
		is_error = TRUE;
		goto clean_up;
	}
	
	/* Check if the keyfile is empty. If it is we delete it */
	gsize count;
	GStrv groups;
	groups = g_key_file_get_groups (keyfile, &count);
	g_strfreev (groups);
	if (count == 0) {
		tmp_error = NULL;
		g_file_delete (file, NULL, &tmp_error);
		
		if (tmp_error) {
			g_set_error (error, GTK_HOTKEY_REGISTRY_ERROR,
						 GTK_HOTKEY_REGISTRY_ERROR_IO,
						 "Failed to delete empty keyfile '%s': %s",
						 path, tmp_error->message);
			is_error = TRUE;
		}
		/* File deleted, we should just clean up and exit */
		goto clean_up;
	}
	
	/* Write new keyfile */
	gsize size;
	gchar *contents;
	tmp_error = NULL;
	contents = g_key_file_to_data (keyfile, &size, &tmp_error);
	if (tmp_error) {
		g_set_error (error, GTK_HOTKEY_REGISTRY_ERROR,
					 GTK_HOTKEY_REGISTRY_ERROR_UNKNOWN,
					 "Failed to generate keyfile contents: %s",
					 tmp_error->message);
		is_error = TRUE;
		goto clean_up;
	}
	
	tmp_error = NULL;
	g_file_set_contents (path, contents, size, &tmp_error);
	if (tmp_error) {
		g_set_error (error, GTK_HOTKEY_REGISTRY_ERROR,
					 GTK_HOTKEY_REGISTRY_ERROR_IO,
					 "Failed to write keyfile '%s': %s",
					 path, tmp_error->message);
		is_error = TRUE;
		goto clean_up;
	}
	
	clean_up:
		if (tmp_error) g_error_free (tmp_error);
		g_object_unref (file);
		g_free (path);
		if (group) g_free (group);
		g_key_file_free (keyfile);
		
	if (is_error)
		return FALSE;
	
	gtk_hotkey_registry_hotkey_deleted (base, info);
	g_object_unref (info);
	return TRUE;
}

static gboolean
gtk_hotkey_key_file_registry_real_has_hotkey (GtkHotkeyRegistry	*base,
											const gchar			*app_id,
											const gchar			*key_id)
{
	GtkHotkeyKeyFileRegistry *self;
	GFile					*file;
	gboolean				exists;
	
	g_return_val_if_fail (app_id != NULL, FALSE);
	g_return_val_if_fail (key_id != NULL, FALSE);
	
	self = GTK_HOTKEY_KEY_FILE_REGISTRY (base);
	
	file = get_hotkey_file (app_id);
	g_return_val_if_fail (G_IS_FILE(file), FALSE);
	
	if (g_file_query_exists (file, NULL))
		exists = TRUE;
	else
		exists = FALSE;
	
	g_object_unref (file);
	return exists;
}

static void
gtk_hotkey_key_file_registry_class_init (GtkHotkeyKeyFileRegistryClass *klass)
{
	gtk_hotkey_key_file_registry_parent_class = g_type_class_peek_parent (klass);
	GTK_HOTKEY_REGISTRY_CLASS (klass)->get_hotkey = gtk_hotkey_key_file_registry_real_get_hotkey;
	GTK_HOTKEY_REGISTRY_CLASS (klass)->get_application_hotkeys = gtk_hotkey_key_file_registry_real_get_application_hotkeys;
	GTK_HOTKEY_REGISTRY_CLASS (klass)->get_all_hotkeys = gtk_hotkey_key_file_registry_real_get_all_hotkeys;
	GTK_HOTKEY_REGISTRY_CLASS (klass)->store_hotkey = gtk_hotkey_key_file_registry_real_store_hotkey;
	GTK_HOTKEY_REGISTRY_CLASS (klass)->delete_hotkey = gtk_hotkey_key_file_registry_real_delete_hotkey;
	GTK_HOTKEY_REGISTRY_CLASS (klass)->has_hotkey = gtk_hotkey_key_file_registry_real_has_hotkey;
}


static void
gtk_hotkey_key_file_registry_init (GtkHotkeyKeyFileRegistry *self)
{
	
}

static void
gtk_hotkey_key_file_registry_finalize (GtkHotkeyKeyFileRegistry *self)
{
	
}

GType
gtk_hotkey_key_file_registry_get_type (void)
{
	static GType gtk_hotkey_key_file_registry_type_id = 0;
	
	if (G_UNLIKELY (gtk_hotkey_key_file_registry_type_id == 0)) {
		static const GTypeInfo g_define_type_info = {
			sizeof (GtkHotkeyKeyFileRegistryClass),
			(GBaseInitFunc) gtk_hotkey_key_file_registry_init,
			(GBaseFinalizeFunc) gtk_hotkey_key_file_registry_finalize,
			(GClassInitFunc) gtk_hotkey_key_file_registry_class_init,
			(GClassFinalizeFunc) NULL,
			NULL,
			sizeof (GtkHotkeyKeyFileRegistry),
			0,
			(GInstanceInitFunc) gtk_hotkey_key_file_registry_init
		};
		
		gtk_hotkey_key_file_registry_type_id = g_type_register_static (GTK_HOTKEY_TYPE_STORAGE, "GtkHotkeyKeyFileRegistry", &g_define_type_info, 0);
	}
	return gtk_hotkey_key_file_registry_type_id;
}

static GFile*
get_hotkey_home (void)
{
	GFile   *home;
	
	home = g_file_parse_name (HOTKEY_HOME);
	
	if (g_file_query_exists(home, NULL) &&
		!gtk_hotkey_g_file_is_directory(home)) {
		g_critical (HOTKEY_HOME" exists but is not a directory");
		g_object_unref (home);
		return NULL;
	}
	
	return home;
}

/* It is not guaranteed that the keyfile exists */
static GFile*
get_hotkey_file (const gchar *app_id)
{
	GFile   *home, *file;
	gchar   *filename;
	
	g_return_val_if_fail (app_id != NULL, NULL);
	
	home = get_hotkey_home();
	g_return_val_if_fail (home != NULL, NULL);
	
	filename = g_strconcat (app_id, HOTKEY_FILE_EXT, NULL);
	file = g_file_get_child (home, filename);
	
	g_object_unref (home);
	g_free (filename);
	return file;
}

static GKeyFile*
get_hotkey_key_file (const gchar *app_id, GError **error)
{
	gchar		*path;
	GFile		*file;
	GKeyFile	*keyfile = NULL;
	GError		*tmp_error;
	
	g_return_val_if_fail (app_id != NULL, NULL);
	g_return_val_if_fail (error == NULL || *error == NULL, NULL);
	
	file = get_hotkey_file (app_id);
	if (!g_file_query_exists (file, NULL)) {
		g_set_error (error, GTK_HOTKEY_REGISTRY_ERROR,
					 GTK_HOTKEY_REGISTRY_ERROR_UNKNOWN_APP,
					 "Application '%s' has not registered any hotkeys", app_id);
		g_object_unref (file);
		return NULL;
	}
	
	path = g_file_get_path (file);
	keyfile = g_key_file_new ();
	
	tmp_error = NULL;
	g_key_file_load_from_file (keyfile, path, 0, &tmp_error);
	if (tmp_error) {
		g_set_error (error, GTK_HOTKEY_REGISTRY_ERROR,
					 GTK_HOTKEY_REGISTRY_ERROR_IO,
					 "Failed to load keyfile '%s': %s", path, tmp_error->message);
		goto clean_up;
	}
	
	clean_up:
		g_free (path);
		g_object_unref (file);
		if (tmp_error) g_error_free (tmp_error);
	
	if (*error) {
		g_key_file_free (keyfile);
		return NULL;
	}
	
	return keyfile;
}

static GtkHotkeyInfo*
get_hotkey_info_from_key_file (GKeyFile	*keyfile,
							   const gchar *app_id,
							   const gchar *key_id,
							   GError **error)
{
	GtkHotkeyInfo   *info = NULL;
	gchar			*group, *description, *app_info_id, *signature;
	GAppInfo		*app_info = NULL;
	
	g_return_val_if_fail (keyfile != NULL, NULL);
	g_return_val_if_fail (error == NULL || *error == NULL, NULL);
	g_return_val_if_fail (app_id != NULL, NULL);
	g_return_val_if_fail (key_id != NULL, NULL);
	
	group = g_strconcat (HOTKEY_GROUP, key_id, NULL);
	description = g_key_file_get_string (keyfile, group, "Description", NULL);
	app_info_id = g_key_file_get_string (keyfile, group, "AppInfo", NULL);
	signature = g_key_file_get_string (keyfile, group, "Signature", NULL);
	
	if (!g_key_file_has_group (keyfile, group)) {
		g_set_error (error, GTK_HOTKEY_REGISTRY_ERROR,
					 GTK_HOTKEY_REGISTRY_ERROR_UNKNOWN_KEY,
					 "Keyfile has no group "HOTKEY_GROUP"%s", key_id);
		goto clean_up;
	}
	
	if (!signature) {
		g_set_error (error, GTK_HOTKEY_REGISTRY_ERROR,
					 GTK_HOTKEY_REGISTRY_ERROR_BAD_SIGNATURE,
					 "No 'Signature' defined for hotkey '%s' for application '%s'",
					 key_id, app_id);
		goto clean_up;
	}
	
	if (app_info_id) {
		app_info = G_APP_INFO(g_desktop_app_info_new (app_info_id));
		if (!G_IS_APP_INFO(app_info)) {
			g_set_error (error, GTK_HOTKEY_REGISTRY_ERROR,
						 GTK_HOTKEY_REGISTRY_ERROR_MISSING_APP,
						 "Keyfile refering to 'AppInfo = %s', but no application"
						 "by that id is registered on the system", app_info_id);
			goto clean_up;
		}	
	}
	
	info = gtk_hotkey_info_new (app_id, key_id, signature, app_info);
	if (description)
		gtk_hotkey_info_set_description (info, description);
	
	clean_up:
		g_free (group);
		if (signature) g_free (signature);
		if (description) g_free (description);
		if (app_info_id) g_free (app_info_id);
		if (app_info) g_object_unref (app_info);
			
	return info;
}

static GList*
get_all_hotkey_infos_from_key_file (GKeyFile	*keyfile,
									const gchar	*app_id)
{
	GList			*hotkeys;
	GtkHotkeyInfo	*hotkey;
	GStrv			groups;
	gsize			count;
	gchar			*group;
	GString			*key_id;
	GError			*error;
	
	g_return_val_if_fail (keyfile != NULL, NULL);
	g_return_val_if_fail (app_id != NULL, NULL);
	
	hotkeys = NULL;
	groups = g_key_file_get_groups (keyfile, &count);
	
	int i;
	for (i = 0; i < count; i++) {
		group = groups[i];
		key_id = g_string_new (group);
		
		/* Ignore non hotkey groups */
		if (!g_str_has_prefix (key_id->str, HOTKEY_GROUP)) {
			g_warning ("Hotkey file for %s contains non 'hotkey:' group '%s'",
					   app_id, group);
			g_string_free (key_id, TRUE);
			continue;
		}
		
		/* Strip 'hotkey:' prefix from key_id */
		g_string_erase (key_id, 0, 7);
		
		error = NULL;
		hotkey = get_hotkey_info_from_key_file (keyfile, app_id, key_id->str, &error);
		if (error) {
			g_warning ("Failed to read hotkey '%s' for application '%s': %s",
					   key_id->str, app_id, error->message);
			g_error_free (error);
			g_string_free (key_id, TRUE);
			continue;
		}
		
		hotkeys = g_list_prepend (hotkeys, hotkey);
			
		g_string_free (key_id, TRUE);
	}
	
	g_strfreev (groups);
	return hotkeys;
}
