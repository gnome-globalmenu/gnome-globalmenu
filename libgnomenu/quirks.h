#ifndef __GNOMENU_QUIRKS_H__
#define __GNOMENU_QUIRKS_H__
G_BEGIN_DECLS
/**
 * SECTION: gnomenu_quirks
 * @short_description: make gnomenu wise.
 * @see_also: #GtkGlobalMenuBar
 * @stability: Unstable
 * @include libgnomenu/quirks.h
 * 
 * GnomenuQuirks handles special treatment for special applications.
 * As a solution to legacy GtkMenuBar application, we patch gtkmenubar.c
 * and modify #gtk_menu_bar_new to create a GtkLegacyMenuBar instead.
 * 
 * GtkLegacyMenuBar is a subclass of GtkGlobalMenuBar(hence a subclass of
 * GtkMenuBar). It overides several function in GtkGlobalMenuBar. The 
 * GtkLegacyMenuBar functions will check the values of the quirk, and decide
 * whether to:
 * forward the call to GtkMenuBar
 * forward the call to GtkGlobalMenuBar
 * fake the call.
 *
 */
#define GNOMENU_TYPE_QUIRKS	(gnomenu_quirks_get_type())
#define GNOMENU_QUIRKS(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GNOMENU_TYPE_QUIRKS, GnomenuQuirks))
#define GNOMENU_QUIRKS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GNOMENU_TYPE_QUIRKS, GnomenuQuirksClass))
#define GNOMENU_IS_QUIRKS(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GNOMENU_TYPE_QUIRKS))
#define GNOMENU_IS_QUIRKS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GNOMENU_TYPE_QUIRKS))
#define GNOMENU_QUIRKS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GNOMENU_TYPE_QUIRKS, GnomenuQuirksClass))

typedef struct _GnomenuQuirksClass GnomenuQuirksClass;
typedef struct _GnomenuQuirks GnomenuQuirks;
typedef enum {
	GNOMENU_QUIRK_MASK_APPNAME,
} GnomenuQuirkMask;
typedef enum {
	GNOMENU_QUIRK_NO_QUIRK = 0x0,
	GNOMENU_QUIRK_ZERO_SIZE = 0x1,
	GNOMENU_QUIRK_POSITION_HACK = 0x2,
} GnomenuQuirkType;
/**
 * GnomenuQuirk:
 *	@app_name: the name of the application;
 *	@type: type of the quirk.
 *	@mask: decides which field in the quirk is used for matching.
 *
 * one entry of quirks. TODO: need more matching critiea;
 */
typedef struct _GnomenuQuirk {
	gchar * app_name;
	GnomenuQuirkType type;
	GnomenuQuirkMask mask;
} GnomenuQuirk;
/**
 * GnomenuQuirks:
 * @quirks: the hash table for all quirks
 */
struct _GnomenuQuirks {
	GObject parent;
	GHashTable * quirks;
};
struct _GnomenuQuirksClass {
	GObjectClass  parent;
};
GnomenuQuirk * gnomenu_quirks_match(GtkMenuBar * menubar); /*FIXME: change this to GtkLegacyMenuBar*/
GnomenuQuirks * gnomenu_quirks_get_default();
GnomenuQuirks * gnomenu_quirks_add_rule(GnomenuQuirks * self, gchar * rule);
GnomenuQuirks * gnomenu_quirks_new_by_file(GnomenuQuirks * self, gchar * filename);
GnomenuQuirks * gnomenu_quirks_new_by_string(GnomenuQuirks * self, gchar * rule);

G_END_DECLS
#endif
