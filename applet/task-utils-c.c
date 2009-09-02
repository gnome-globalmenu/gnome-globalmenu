#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include <glib.h>

char* get_task_name_by_pid(int pid) {
	char linkname[64];
	int ret;
	//snprintf(linkname, sizeof(linkname), "/proc/%i/exe", pid);
	snprintf(linkname, sizeof(linkname), "/proc/%i/cmdline", pid);

	char buf[64];
	FILE* file = fopen(linkname, "r");
	
	if (file != NULL) {
		ret = fread(buf, 1, 64, file);
		fclose(file);
	} else ret = readlink(linkname, buf, 64);

	if (ret < 1) {
		char linkname2[64];
		snprintf(linkname2, sizeof(linkname2), "/proc/%i/exe", pid);
		ret = readlink(linkname, buf, 64);
	}
	
	if (ret == -1)
		return NULL;
	buf[ret] = 0;
	
	GString* r = g_string_new("");
	int co;
	for(co = 0; co<=ret; co++) {
		if ((int)buf[co]>31) 
			r = g_string_append_c(r, buf[co]); else
			r = g_string_append_c(r, ' ');
	}
		
	return g_string_free(r, FALSE);
}
