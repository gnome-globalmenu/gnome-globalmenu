#include <gtk/gtk.h>
#if GTK_MINOR_VERSION < 12
#define gtk_about_dialog_set_program_name(dlg, name)
#define gtk_widget_set_tooltip_text(widget, text)
#endif
