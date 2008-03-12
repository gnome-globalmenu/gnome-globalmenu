#include <gtk/gtk.h>

#include <config.h>

#include "quirks.h"
#include "menubar.h"

#if ENABLE_TRACING >= 3
#define LOG(fmt, args...) g_message("<GnomenuQuirk>::" fmt, ## args)
#else
#define LOG(fmt, args...)
#endif

#define LOG_FUNC_NAME LOG("%s", __func__)

/**
 * QuirkEntry:
 * 	@match: string to match application name(#g_prgname()).
 *	@detail: string to match detail 
 *		(role or title of the window that the menubar belongs to);
 * 	@mask: mask.
 * */
typedef struct {
	gchar * match;
	gchar * detail;
	GnomenuQuirkMask mask;
} QuirkEntry;

static GFlagsValue * _get_quirk_value_by_nick(gchar * nick){
	static GTypeClass * type_class = NULL;
		if(type_class == NULL) type_class = g_type_class_ref(gnomenu_quirk_mask_get_type());
	return g_flags_get_value_by_nick(type_class, nick);
}
static void _add_quirks_from_string(GQueue * quirks, gchar * string){
	gchar ** lines;
	gchar ** words;
	gchar * word;
	gchar ** str_quirks;
	gchar ** details;
	int i, j, k, l;
	QuirkEntry * entry ;
	lines = g_strsplit(string, "\n", 0);
	if(lines)
	for(i = 0; lines[i]; i++){
		words = g_strsplit(lines[i], ":", 0);
		if(!words) continue;
		l = g_strv_length(words);
		if(l == 0) continue;
		word = g_strstrip(words[0]);
		if(!word || word[0] == '#'){
			g_strfreev(words);
			continue;
		}
		if(l !=2 ){
			g_warning("Irregular conf file line(%d):\n%s", l, lines[i]);
			g_strfreev(words);
			continue;
		}
		details = g_strsplit(word, "/", 2);
		if(details){
			entry = g_new0(QuirkEntry, 1);
			entry->match = g_strdup(g_strstrip(details[0]));
			if(details[1])
				entry->detail = g_strdup(g_strstrip(details[1]));
			else 
				entry->detail = NULL; /*default quirk*/
		} else {
			g_strfreev(words);
			continue;
		}
		word = g_strstrip(words[1]);
		entry->mask = GNOMENU_QUIRK_NONE;
		str_quirks = g_strsplit(word, ",", 0);
		if(str_quirks){
			l = g_strv_length(str_quirks);
			for(k = 0; k < l; k++){
				word = g_strstrip(str_quirks[k]);
				GFlagsValue * value = _get_quirk_value_by_nick(word);
				if(value){
					entry->mask |= value->value;
					LOG("found quirk for %s: %s=%d", entry->match, word, value->value);
				} else {
					g_warning("unknown quirk type: %s", word);
				}
			}
			g_strfreev(str_quirks);
		}
		g_queue_push_tail(quirks, entry);	
		LOG("new quirk %s/%s = %d", entry->match, entry->detail, entry->mask);
	}
	g_strfreev(lines);
}
static void _add_quirks_from_file(GQueue * quirks, gchar * file){
	gchar * contents;
	gint length;
	GError * error = NULL;
	if(g_file_get_contents(file, &contents, &length, &error)) {
		LOG("file opened");
		_add_quirks_from_string(quirks, contents);
	}else{
		LOG("%s", error->message);
	}
	g_free(contents);
}
static GQueue * _get_quirks(){
	static GQueue * quirks = NULL;
	if(!quirks) {
		gchar * file;
		quirks = g_queue_new();
		_add_quirks_from_file(quirks, SYSCONFDIR G_DIR_SEPARATOR_S "libgnomenu.conf");
		file = g_strconcat(g_get_home_dir(), G_DIR_SEPARATOR_S ".libgnomenu.conf", NULL);
		_add_quirks_from_file(quirks, file);
	}
	return quirks;
}
struct quirk_match_data {
	gchar * prgname;
	gchar * detail;
	GnomenuQuirkMask rt;
};
static void _match_quirk(QuirkEntry * entry, struct quirk_match_data * data){
	LOG("match %s/%s against %s/%s", data->prgname, 
			data->detail,
			entry->match, 
			entry->detail);
	if(g_pattern_match_simple(entry->match, data->prgname)) {
		if(data->detail && entry->detail){
			if(g_pattern_match_simple(entry->detail, data->detail))
				data->rt |= entry->mask;
		}
		if(data->detail && !entry->detail){ /*matching against a default*/
			data->rt |= entry->mask;
		}
		if(!data->detail && !entry->detail){ /* default against default*/
			data->rt |= entry->mask;
		}
		if(!data->detail && entry->detail){
			/*default againser non-default*/
		}
	}
}
/**
 * gnomenu_get_default_quirk:
 *
 * Find the default quirk.
 *
 * Returns: the default quirk of current application.
 */
GnomenuQuirkMask gnomenu_get_default_quirk(){
	static GnomenuQuirkMask default_quirk = GNOMENU_QUIRK_NONE;
	static gboolean loaded = FALSE;
	if(!loaded){
		struct quirk_match_data data = { g_get_prgname(), NULL, GNOMENU_QUIRK_NONE};
		g_queue_foreach(_get_quirks(), _match_quirk, &data);
		default_quirk = data.rt;
		LOG("default_quirk = %d", default_quirk);
		loaded = TRUE;
	}	
	return default_quirk;	
}
/**
 * gnomenu_get_detail_quirk:
 *	@detail: detail string of the quirk.
 *
 * Find the detail quirk.
 *
 * Returns: the quirk for the given detail. 'detail' is usually
 * the title or the role of the window.
 */
GnomenuQuirkMask gnomenu_get_detail_quirk(gchar * detail){
	struct quirk_match_data data = { g_get_prgname(), detail, GNOMENU_QUIRK_NONE};
	g_queue_foreach(_get_quirks(), _match_quirk, &data);
	return data.rt;
}
