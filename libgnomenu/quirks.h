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
typedef enum { /*< prefix = GNOMENU_QUIRK >*/
	GNOMENU_QUIRK_NONE = 0, /*< nick=none >*/
	GNOMENU_QUIRK_IGNORE = 1<<0, /*< nick=ignore >*/
	GNOMENU_QUIRK_FORCE_SHOW_ALL = 1<<1, /*< nick=force-show-all >*/
	GNOMENU_QUIRK_HIDE_ON_QUIT = 1<<2, /*< nick=hide-on-quit >*/
} GnomenuQuirkMask;
#define GNOMENU_HAS_QUIRK(m, v) ((m) & (GNOMENU_QUIRK_ ## v))
GnomenuQuirkMask gnomenu_get_default_quirk(); 
GnomenuQuirkMask gnomenu_get_detail_quirk(gchar * detail);

GtkMenuBar * gnomenu_menu_bar_new_legacy(const char * first_property, ...);
#define GNOMENU_TYPE_QUIRK_MASK gnomenu_quirk_mask_get_type()
G_END_DECLS
#endif
