#include <gtk/gtk.h>

#include <config.h>

#include "quirks.h"
#include "menubar.h"

#define LOG(fmt, args...) g_message("<GnomenuQuirk>::" fmt, ## args)
#define LOG_FUNC_NAME LOG("%s", __func__)

static GQueue * default_quirks = NULL;
typedef struct {
	gchar * match;
	GnomenuQuirkMask mask;
} QuirkEntry;

static void _add_default_quirks_from_string(gchar * string){
	gchar ** lines;
	gchar ** words;
	gchar * word;
	int i, j, l;
	lines = g_strsplit(string, "\n", 0);
	for(i = 0; lines[i]; i++){
		QuirkEntry * entry = g_new0(QuirkEntry, 1);
		words = g_strsplit(lines[i], ":", 0);
		if(!words) continue;
		l = g_strv_length(words);
		if(l !=2){
			g_warning("Irregular conf file line(%d):\n%s", l, lines[i]);
			g_strfreev(words);
			continue;
		}
		word = g_strstrip(words[0]);
		if(word[0] == '#'){
			g_strfreev(words);
			g_free(word);
			continue;
		}
		entry->match = word;
		word = g_strstrip(words[1]);
		entry->mask = GNOMENU_QUIRK_NONE;
		if(g_str_equal(word, "ignore")){
			entry->mask = GNOMENU_QUIRK_IGNORE;
		} else 
			g_warning("Unknown quirk type: %s", word);
		g_queue_push_tail(default_quirks, entry);	
		LOG("new quirk: %s : %s", entry->match, word);
		g_free(word);
	}
	g_strfreev(lines);
}
static void _add_default_quirks_from_file(gchar * file){
	gchar * contents;
	gint length;
	GError * error = NULL;
	if(g_file_get_contents(file, &contents, &length, &error)) {
		LOG("file opened");
		_add_default_quirks_from_string(contents);
	}else{
		LOG("%s", error->message);
	}
	g_free(contents);
}
static void _load_default_quirks(){
	if(!default_quirks) {
		gchar * file;
		default_quirks = g_queue_new();
		_add_default_quirks_from_file(SYSCONFDIR G_DIR_SEPARATOR_S "libgnomenu.conf");
		file = g_strconcat(g_get_home_dir(), G_DIR_SEPARATOR_S ".libgnomenu.conf", NULL);
		_add_default_quirks_from_file(file);
	}
}
struct quirk_match_data {
	gchar * prgname;
	GnomenuQuirkMask rt;
};
static void _match_quirk(QuirkEntry * entry, struct quirk_match_data * data){
	LOG("match %s against %s", data->prgname, entry->match);
	if(g_pattern_match_simple(entry->match, data->prgname)) {
		data->rt = GNOMENU_QUIRK_IGNORE;
	}
}
/**
 * gnomenu_get_default_quirk:
 *
 * Returns: the default quirk for current application
 */
GnomenuQuirkMask gnomenu_get_default_quirk(){
	_load_default_quirks();
	QuirkEntry * entry;
	struct quirk_match_data data = { g_get_prgname(), GNOMENU_QUIRK_NONE};
	g_queue_foreach(default_quirks, _match_quirk, &data);
	
	return data.rt;	
}
GType gnomenu_menu_bar_type = 0;
void gtk_module_init(int * argc, char **argv[]){
/*initialize */
	switch(gnomenu_get_default_quirk()){
		case GNOMENU_QUIRK_IGNORE:
		break;
		case GNOMENU_QUIRK_NONE:
		default:
			gnomenu_menu_bar_type = gnomenu_menu_bar_get_type();
	}
}
