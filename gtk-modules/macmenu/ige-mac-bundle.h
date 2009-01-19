/* GTK+ Integration for app bundles.
 *
 * Copyright (C) 2007-2008 Imendio AB
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1
 * of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __IGE_MAC_BUNDLE_H__
#define __IGE_MAC_BUNDLE_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define IGE_TYPE_MAC_BUNDLE            (ige_mac_bundle_get_type ())
#define IGE_MAC_BUNDLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), IGE_TYPE_MAC_BUNDLE, IgeMacBundle))
#define IGE_MAC_BUNDLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), IGE_TYPE_MAC_BUNDLE, IgeMacBundleClass))
#define IGE_IS_MAC_BUNDLE(obj)	       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IGE_TYPE_MAC_BUNDLE))
#define IGE_IS_MAC_BUNDLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), IGE_TYPE_MAC_BUNDLE))
#define IGE_MAC_BUNDLE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), IGE_TYPE_MAC_BUNDLE, IgeMacBundleClass))

typedef struct _IgeMacBundle      IgeMacBundle;
typedef struct _IgeMacBundleClass IgeMacBundleClass;

struct _IgeMacBundle {
  GObject parent_instance;
};

struct _IgeMacBundleClass {
  GObjectClass parent_class;
};

GType         ige_mac_bundle_get_type          (void);
IgeMacBundle *ige_mac_bundle_new               (void);
IgeMacBundle *ige_mac_bundle_get_default       (void);
void          ige_mac_bundle_setup_environment (IgeMacBundle *bundle);
const gchar * ige_mac_bundle_get_id            (IgeMacBundle *bundle);
const gchar * ige_mac_bundle_get_path          (IgeMacBundle *bundle);
gboolean      ige_mac_bundle_get_is_app_bundle (IgeMacBundle *bundle);
const gchar * ige_mac_bundle_get_localedir     (IgeMacBundle *bundle);
const gchar * ige_mac_bundle_get_datadir       (IgeMacBundle *bundle);
gchar *       ige_mac_bundle_get_resource_path (IgeMacBundle *bundle,
                                                const gchar  *name,
                                                const gchar  *type,
                                                const gchar  *subdir);

G_END_DECLS

#endif /* __IGE_MAC_BUNDLE_H__ */
