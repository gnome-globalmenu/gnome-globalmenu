typedef enum {
	GM_NOTIFY_NOT_GM,
	GM_NOTIFY_MIN,
	GM_NOTIFY_NEW,
	GM_NOTIFY_DESTROY,
	GM_NOTIFY_SERVER_NEW,
	GM_NOTIFY_SERVER_DESTROY,
	GM_NOTIFY_SIZE_ALLOCATION,
	GM_NOTIFY_MAX,
} GlobalMenuNotifyType;
#define ATOM_STRING "_GTKMENUBAR_EMBED"
#define SERVER_NAME "GTK MENU BAR SERVER"
#define CLIENT_NAME "GTK MENU BAR CLIENT"

typedef struct {
	GlobalMenuNotifyType type;
	union{
	struct {
		gulong param1;
		gulong param2;
		gulong param3;
	}; /*general*/
		struct {
			Window client_xid;
			Window float_xid;
			Window master_xid;
		} ClientNew;
		struct {
			Window client_xid;
			Window float_xid;
			Window master_xid;
		} ClientDestroy;
		struct {
			Window server_xid;
		} ServerNew;
		struct {
			Window server_xid;
		} ServerDestroy;
		struct {
			Window server_xid;
			glong width;
			glong height;
		} SizeAllocation;
	};
} GlobalMenuNotify;

typedef void (*GlobalMenuCallback)(GlobalMenuSocket* socket, 
	GlobalMenuNotify * notify, gpointer data);

typedef struct {
	gchar * name;
	GdkWindow * window;
	GdkDisplay * display;
	GlobalMenuCallback callbacks[GM_NOTIFY_MAX];
	gpointer userdata;
	Window dest_xid; // Where to connect to
} GlobalMenuSocket;

static const gchar * global_menu_notify_get_name(GlobalMenuNotifyType type){
#define CASE(x) case x: return # x;
	switch(type){
	CASE(GM_NOTIFY_NOT_GM)
	CASE(GM_NOTIFY_NEW)
	CASE(GM_NOTIFY_DESTROY)
	CASE(GM_NOTIFY_SERVER_NEW)
	CASE(GM_NOTIFY_SERVER_DESTROY)
	CASE(GM_NOTIFY_SIZE_ALLOCATION)
	default:
		return "Unknown notification";
	}
#undef CASE
}

static gboolean global_menu_xevent_to_notify(XEvent * xevent, GlobalMenuNotify * notify){
	GdkDisplay * display = NULL;

	if(notify == NULL){ 
		g_warning("notify is NULL\n");
		return FALSE;
	}

	notify->type = GM_NOTIFY_NOT_GM;
	if(xevent->type == ClientMessage){

		display = gdk_x11_lookup_xdisplay(xevent->xclient.display);
		if(display == NULL){
			g_warning("Message not from a gdk managed display, ignore it\n");
			return FALSE;		
		}
		if( xevent->xclient.message_type == 
			gdk_x11_get_xatom_by_name_for_display(display, ATOM_STRING)){
			notify->type = xevent->xclient.data.l[1];
			notify->param1 = xevent->xclient.data.l[2];
			notify->param2 = xevent->xclient.data.l[3];
			notify->param3 = xevent->xclient.data.l[4];
			g_message("Global Menu Notification: %s, %d, %d, %d\n",
				global_menu_notify_get_name(notify->type),
					notify->param1, notify->param2, notify->param3);
			return TRUE;
		}
	}
	return FALSE;
}

global_menu_socket_dispatcher(XEvent * xevent, GdkEvent * event, GlobalMenuSocket * socket){
	GlobalMenuNotify notify;
	GlobalMenuNotifyType type;
	if(global_menu_translate_xevent(xevent, &notify)){
		for(type = GM_NOTIFY_MIN + 1; type < GM_NOTIFY_MAX; type++){
			if(socket->callbacks[type]){
				(*(socket->callbacks[type]))(socket, &notify, socket->userdata);
			}
		}
		return GDK_FILTER_REMOVE;
	}
	return GDK_FILTER_CONTINUE;
	
}

GlobalMenuSocket * global_menu_socket_new(gchar * name, gpointer userdata){
	GdkWindowAttr attr;
	GdkWindowAttributesType mask;

	GlobalMenuSocket * socket = g_new0(GlobalMenuSocket, 1);
	attr.title = name;
	attr.wclass = GDK_INPUT_ONLY;
	attr.window_type = GDK_WINDOW_TOP_LEVEL;

	mask = GDK_WA_TITLE;
	socket->window = gdk_window_new(NULL, &attr, mask);
	socket->name = g_strdup(name);
	socket->userdata = userdata,
	socket->display = gdk_drawable_get_display(socket->window);

	gdk_window_add_filter(socket->window, global_menu_socket_dispatcher, socket);

	return socket;
}
void global_menu_socket_free(GlobalMenuSocket * socket){
	gdk_window_destroy(socket->window);
	g_free(socket->name);
	g_free(socket);
}
void global_menu_socket_set_callback(GlobalMenuSocket * socket, 
		GlobalMenuNotifyType type, GlobalMenuCallback cb){
	g_return_if_fail( type > GM_NOTIFY_MIN && type < GM_NOTIFY_MAX);
	g_return_if_fail( socket );
	g_return_if_fail( cb );
	
	socket->callbacks[type] = cb;
}
void global_menu_socket_connect(GlobalMenuSocket * socket, gchar * dest_name){
	GdkScreen * screen;
	GdkDisplay * display;
	GList * windows;
	GList * node;
	GdkWindow * dest = NULL;

	g_return_if_fail( socket );
	g_return_if_fail( dest_name );
	
	screen = gdk_drawable_get_screen(socket->window);

	g_return_if_fail( screen );	

	node = windows = gdk_screen_get_top_levels(screen);
		
	while(node){
		Atom type_return;
		Atom type_req = gdk_x11_get_xatom_by_name_for_display (socket->display, "UTF8_STRING"),
		gint format_return;
		gulong nitems_return;
		gulong bytes_after_return;
		gchar * data;

		GdkWindow * window = node->data;
		g_assert(window);	
		if(XGetWindowProperty (GDK_DISPLAY_XDISPLAY (socket->display), GDK_WINDOW_XID (window),
						  gdk_x11_get_xatom_by_name_for_display (socket->display, "_NET_WM_NAME"),
                          0, G_MAXLONG, False, type_req, &type_return,
                          &format_return, &nitems_return, &bytes_after_return,
                          &data) == Success)
		if(type_return == type_req){
			g_print("Window name is %s\n", 	data);
			if(g_str_equal(dest_name, data)){
				g_print("dest found");
				dest = window;
				break;
			}
		}
		node = node->next;
	}
	if(windows) g_list_free(windows);	
	g_return_if_fail(dest);
	socket->dest_xid = GDK_WINDOW_XID(dest);
}

void global_menu_socket_send(GlobalMenuSocket * socket, GlobalMenuNotify * message){
	XClientMessageEvent xclient;

	memset (&xclient, 0, sizeof (xclient));
	xclient.window = socket->dest_xid;
	xclient.type = ClientMessage;
	xclient.message_type = gdk_x11_get_xatom_by_name_for_display (display, ATOM_STRING);
	xclient.format = 32;
	xclient.data.l[0] = gtk_get_current_event_time();
	xclient.data.l[1] = message->type;
	xclient.data.l[2] = message->param1;
	xclient.data.l[3] = message->param2;
	xclient.data.l[4] = message->param3;
	gdk_error_trap_push ();
	XSendEvent (GDK_DISPLAY_XDISPLAY(socket->display),
		  socket->dest_xid,
		  False, NoEventMask, (XEvent *)&xclient);
	gdk_display_sync (socket->display);
	gdk_error_trap_pop ();
}
