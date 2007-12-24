typedef gulong HandlerID;
typedef Window XWindowID;

typedef struct _Application Application;
typedef struct {
	gchar * Title;
	gboolean IsDummy;
	gboolean IsDead; /*Set if Socket is destroyed*/
	union {
		GtkWidget * Widget;
		struct {
			GtkSocket * Socket;
			XWindowID MasterWID;
			GdkPixbuf * Icon;
		};
	}/* Menubar*/;
	struct {
	gint x;
	gint y;
	gint w;
	gint h;
	}/* Geometry*/;
	struct {
		HandlerID destroy;
	} Handlers;
	struct _Application * App;
} ClientEntry;

struct _Application {
	GtkWindow * MainWindow;
	struct {
		GtkImage * ClientIcon;
		GtkLabel * TitleLabel;
		GtkLayout * Layout;
		GtkNotebook * Notebook; /*menu collection, contains all the sockets*/
	}; /*Widgets*/
	GHashTable * Clients; /*hashed by Client's window id*/
	WnckScreen * Screen;
	ClientEntry * ActiveClient;
	struct {
		HandlerID active_window_changed;
		HandlerID window_opened;
	} Handlers;
};
