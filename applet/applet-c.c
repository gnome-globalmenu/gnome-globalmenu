#include <gtk/gtk.h>
#include <panel-2.0/panel-applet.h>
GObject * gnome_program_init_easy(char * name, char * version, 
		char ** argv, int argc, GOptionContext * context) {
	GnomeProgram * program;
	program = gnome_program_init (name, version,				
				      LIBGNOMEUI_MODULE,			
				      argc, argv,				
				      GNOME_PARAM_GOPTION_CONTEXT, context,	
				      GNOME_CLIENT_PARAM_SM_CONNECT, FALSE,	
				      GNOME_PROGRAM_STANDARD_PROPERTIES,	
				      NULL);
	return program;

}
