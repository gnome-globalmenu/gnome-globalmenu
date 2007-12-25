typedef gulong HandlerID;
typedef Window XWindowID;

typedef struct _Application Application;
typedef struct {
	gchar * Title;
	enum {
		MENUBAR_LOCAL,
		MENUBAR_REMOTE
	} Type;
	gboolean IsDead; /*Set if Socket is destroyed*/
	union {
		struct {
			gboolean IsDummy;
			GtkWidget * Widget;
		};/* local menubars*/
		struct {
			GtkSocket * Socket;
			XWindowID MasterWID;
			GdkPixbuf * Icon;
			gboolean IsStolen;
		}; /*for remote menubars*/
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
	GtkContainer * MainWindow;
	enum AppMode {
		APP_STANDALONE,
		APP_APPLET
	} Mode;
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
