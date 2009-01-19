/* GTK+ Integration for the Mac OS X Dock.
 *
 * Copyright (C) 2007, 2008 Imendio AB
 *
 * For further information, see:
 * http://developer.imendio.com/projects/gtk-macosx/integration
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

#ifndef __IGE_MAC_DOCK_H__
#define __IGE_MAC_DOCK_H__

#include <gtk/gtk.h>
#include <ige-mac-bundle.h>

G_BEGIN_DECLS

#define IGE_TYPE_MAC_DOCK            (ige_mac_dock_get_type ())
#define IGE_MAC_DOCK(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), IGE_TYPE_MAC_DOCK, IgeMacDock))
#define IGE_MAC_DOCK_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), IGE_TYPE_MAC_DOCK, IgeMacDockClass))
#define IGE_IS_MAC_DOCK(obj)	     (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IGE_TYPE_MAC_DOCK))
#define IGE_IS_MAC_DOCK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), IGE_TYPE_MAC_DOCK))
#define IGE_MAC_DOCK_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), IGE_TYPE_MAC_DOCK, IgeMacDockClass))

typedef struct _IgeMacDock      IgeMacDock;
typedef struct _IgeMacDockClass IgeMacDockClass;

typedef struct _IgeMacAttentionRequest IgeMacAttentionRequest;

struct _IgeMacDock
{
  GObject parent_instance;
};

struct _IgeMacDockClass
{
  GObjectClass parent_class;
};

typedef enum {
        IGE_MAC_ATTENTION_CRITICAL,
        IGE_MAC_ATTENTION_INFO
} IgeMacAttentionType;

GType                   ige_mac_dock_get_type                  (void);
IgeMacDock *            ige_mac_dock_new                       (void);
IgeMacDock *            ige_mac_dock_get_default               (void);
void                    ige_mac_dock_set_icon_from_pixbuf      (IgeMacDock             *dock,
                                                                GdkPixbuf              *pixbuf);
void                    ige_mac_dock_set_icon_from_resource    (IgeMacDock             *dock,
                                                                IgeMacBundle           *bundle,
                                                                const gchar            *name,
                                                                const gchar            *type,
                                                                const gchar            *subdir);
void                    ige_mac_dock_set_overlay_from_pixbuf   (IgeMacDock             *dock,
                                                                GdkPixbuf              *pixbuf);
void                    ige_mac_dock_set_overlay_from_resource (IgeMacDock             *dock,
                                                                IgeMacBundle           *bundle,
                                                                const gchar            *name,
                                                                const gchar            *type,
                                                                const gchar            *subdir);
IgeMacAttentionRequest *ige_mac_dock_attention_request         (IgeMacDock             *dock,
                                                                IgeMacAttentionType     type);
void                    ige_mac_dock_attention_cancel          (IgeMacDock             *dock,
                                                                IgeMacAttentionRequest *request);

#define IGE_TYPE_MAC_ATTENTION_TYPE (ige_mac_attention_type_get_type())
GType                   ige_mac_attention_type_get_type        (void);

G_END_DECLS

#endif /* __IGE_MAC_DOCK_H__ */
