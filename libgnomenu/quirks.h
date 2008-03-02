#ifndef __GNOMENU_QUIRKS_H__
#define __GNOMENU_QUIRKS_H__
G_BEGIN_DECLS
/**
 * SECTION: quirks
 * 	@title: Dealing with Quirks
 * 	@short_description: Give a little wisdom to libgnomenu.
 * 	@see_also: #GnomenuMenuBar
 * 	@stability: Unstable
 * 	@include: libgnomenu/quirks.h
 * 
 * Quirks is the special treatment for special applications. We need quirks
 * only because we need to stand-in for GtkMenuBar. 
 *
 * Quirks are specified in libgnomenu.conf, located at ~/.libgnomenu.conf
 * and /etc/libgnomenu.conf.(%sysconfdir).
 * 
 * There are two kind of quirks. Default quirk and detail quirk.
 * Default quirk matches for application's g_prgname(),
 * Detail quirk matches not only for applicatoin's g_prgname, but also
 * for a detail string, given by #GnomenuMenuBar. The string should be the
 * title or the role of the menubar's toplevel window.
 */

/**
 * GnomenuQuirkMask:
 * 	@GNOMENU_QUIRK_NONE:	No quirk needed.
 * 	@GNOMENU_QUIRK_IGNORE:	Don't replace GtkMenuBar with GnomenuMenuBar
 * 	@GNOMENU_QUIRK_FORCE_SHOW_ALL:	
 * 		Issue a gtk_widget_show_all when server requests GnomenuMenuBar
 * 		to show up. (For nautilus/Desktop)
 * 	@GNOMENU_QUIRK_HIDE_ON_QUIT:
 * 		hide itself when the server quits. (For nautilus/Desktop)
 * 	@GNOMENU_QUIRK_ROAMING:
 * 		When there is no server, menu bar goes into a seperate 
 * 		toplevel window instead of go back to app window. (Fow wxWindows).
 *
 * 	Different types of quirks. Used by #GnomenuMenuBar.
 */
typedef enum { /*< prefix = GNOMENU_QUIRK >*/
	GNOMENU_QUIRK_NONE = 0, /*< nick=none >*/
	GNOMENU_QUIRK_IGNORE = 1<<0, /*< nick=ignore >*/
	GNOMENU_QUIRK_FORCE_SHOW_ALL = 1<<1, /*< nick=force-show-all >*/
	GNOMENU_QUIRK_HIDE_ON_QUIT = 1<<2, /*< nick=hide-on-quit >*/
	GNOMENU_QUIRK_ROAMING	= 1<<3, /*< nick=roaming >*/
} GnomenuQuirkMask;
/**
 * GNOMENU_HAS_QUIRK:
 * 	@m: a #GnomenuQuirkMask
 * 	@v:	a quirk name stripping the GNOMENU_QUIRK_ prefix
 *
 * Test if quirk @m has quirk type @v. 
 *
 * NOTE: @v don't need GNOMEN_QUIRK_ prefix.
 */
#define GNOMENU_HAS_QUIRK(m, v) ((m) & (GNOMENU_QUIRK_ ## v))
GnomenuQuirkMask gnomenu_get_default_quirk(); 
GnomenuQuirkMask gnomenu_get_detail_quirk(gchar * detail);

/**
 * gnomenu_compatible:
 *
 * 	TRUE if works in gtk compatible mode
 */
extern gboolean gnomenu_compatible;
/*< private: >*/
#define GNOMENU_TYPE_QUIRK_MASK gnomenu_quirk_mask_get_type()

G_END_DECLS
#endif
