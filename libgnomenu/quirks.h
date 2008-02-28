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
	GNOMENU_QUIRK_NONE,
	GNOMENU_QUIRK_IGNORE,
	GNOMENU_QUIRK_CLASS,
} GnomenuQuirkMask;
GnomenuQuirkMask gnomenu_get_default_quirk(); 
GtkMenuBar * gtk_legacy_menu_bar_new();
G_END_DECLS
#endif
