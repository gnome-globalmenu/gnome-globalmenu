#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include <glib.h>

char* __get_task_name_by_pid(int pid) {
	char linkname[64];
	int ret;
	snprintf(linkname, sizeof(linkname), "/proc/%i/exe", pid);
	
	char buf[64];
	ret = readlink(linkname, buf, 64);
	if (ret == -1)
		return NULL;
	buf[ret] = 0;
	
	GString* r = g_string_new("");
	int co;
	for(co = 0; co<=ret; co++)
		r = g_string_append_c(r, buf[co]);
		
	return r->str;
}
