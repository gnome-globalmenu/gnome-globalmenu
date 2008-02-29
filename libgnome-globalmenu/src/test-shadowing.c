#include <gtk/gtk.h>

void main(){
	void * x;
	x = gtk_menu_bar_new();
	g_print("menu bar new returns: %p\n", x);
}
