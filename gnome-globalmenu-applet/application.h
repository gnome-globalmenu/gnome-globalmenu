struct _WnckScreen;
struct _ClientInfo;
struct _Application {
	GtkContainer * MainWindow;
	struct _MenuServer * Server;
	struct {
		GtkImage * ClientIcon;
		GtkLabel * TitleLabel;
		GtkFixed * Holder; /*since it is the only widget i know who can has a window*/
		GtkEventBox * Backward;
		GtkEventBox * Forward;
	}; /*Widgets*/
	struct {
		GdkPixmap * Background;
		GdkColor Color;
		gboolean show_title; /*If show current window's title*/
		gboolean show_icon; /*current window's icon*/
/*The above two property has no effect, since not implemented yet.*/
		gboolean show_arrows; /*whether show the stupid scrolling arrows*/
		gint title_max_width_chars; /*max width of the label in chars*/
	} AppletProperty;
	struct _WnckScreen * Screen;
	struct _ClientInfo * ActiveClient;
	gchar * ActiveTitle;
	GdkPixbuf * ActiveIcon;
	GList * Clients;
};

typedef struct _Application Application;
