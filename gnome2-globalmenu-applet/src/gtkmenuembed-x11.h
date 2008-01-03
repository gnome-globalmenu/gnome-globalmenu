typedef enum {
	GM_NOTIFY_NOT_GM,
	GM_NOTIFY_MIN,
	GM_NOTIFY_NEW,
	GM_NOTIFY_DESTROY,
	GM_NOTIFY_SERVER_NEW,
	GM_NOTIFY_SERVER_DESTROY,
	GM_NOTIFY_SIZE_ALLOCATE,
	GM_NOTIFY_SET_VISIBLE,
	GM_NOTIFY_MAX,
} GlobalMenuNotifyType;
#define ATOM_STRING "_GTKMENUBAR_EMBED"
#define MENU_SERVER_NAME "GTK MENU BAR SERVER"
#define MENU_CLIENT_NAME "GTK MENU BAR CLIENT"

typedef struct _GlobalMenuNotify {
	GlobalMenuNotifyType type;
	union{
	struct {
		gulong param1; /*always set to be the source socket's window id*/
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
		} SizeAllocate;
		struct {
			Window server_xid;
			gboolean visible;
		} SetVisible;
	};
} GlobalMenuNotify;

typedef struct _GlobalMenuSocket GlobalMenuSocket;

typedef void (*GlobalMenuCallback)(GlobalMenuSocket* socket, 
	GlobalMenuNotify * notify, gpointer data);

struct _GlobalMenuSocket {
	gchar * name;
	GdkWindow * window;
	GdkDisplay * display;
	GlobalMenuCallback callbacks[GM_NOTIFY_MAX];
	gpointer userdata;
	Window dest_xid; // Where to connect to
};


#ifdef INCLUDE_SOURCE
static const gchar * global_menu_notify_get_name(GlobalMenuNotifyType type){
#define CASE(x) case x: return # x;
	switch(type){
	CASE(GM_NOTIFY_NOT_GM)
	CASE(GM_NOTIFY_NEW)
	CASE(GM_NOTIFY_DESTROY)
	CASE(GM_NOTIFY_SERVER_NEW)
	CASE(GM_NOTIFY_SERVER_DESTROY)
	CASE(GM_NOTIFY_SIZE_ALLOCATE)
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
		g_message("XClient notify translating");

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
			g_message("Global Menu Notification: %s, %ld, %ld, %ld\n",
				global_menu_notify_get_name(notify->type),
					notify->param1, notify->param2, notify->param3);
			return TRUE;
		}
	}
	return FALSE;
}

static GdkFilterReturn global_menu_socket_dispatcher(XEvent * xevent, GdkEvent * event, GlobalMenuSocket * socket){
	GlobalMenuNotify notify;
	GlobalMenuNotifyType type;
	g_message("XClientMessage received");
	if(event->any.window == socket->window 
		&& global_menu_xevent_to_notify(xevent, &notify)){
		type = notify.type;
		if(socket->callbacks[type] ){
			(*(socket->callbacks[type]))(socket, &notify, socket->userdata);
		}
		return GDK_FILTER_REMOVE;
	} else{
	g_message("Pass to futher handlers, not a MenuBar message");

	}
	return GDK_FILTER_CONTINUE;
	
}

static GlobalMenuSocket * global_menu_socket_new(gchar * name, gpointer userdata){
	GdkWindowAttr attr;
	GdkWindowAttributesType mask;

	GlobalMenuSocket * socket = g_new0(GlobalMenuSocket, 1);
	attr.title = name;
	attr.wclass = GDK_INPUT_ONLY;
	attr.window_type = GDK_WINDOW_TEMP;

	mask = GDK_WA_TITLE;
	socket->window = gdk_window_new(NULL, &attr, mask);
	socket->name = g_strdup(name);
	socket->userdata = userdata,
	socket->display = gdk_drawable_get_display(socket->window);
	
	gdk_window_add_filter(socket->window, (GdkFilterFunc)global_menu_socket_dispatcher, socket);

	return socket;
}
static void global_menu_socket_free(GlobalMenuSocket * socket){
	gdk_window_destroy(socket->window);
	g_free(socket->name);
	g_free(socket);
}
static void global_menu_socket_set_callback(GlobalMenuSocket * socket, 
		GlobalMenuNotifyType type, GlobalMenuCallback cb){
	g_return_if_fail( type > GM_NOTIFY_MIN && type < GM_NOTIFY_MAX);
	g_return_if_fail( socket );
	g_return_if_fail( cb );
	
	socket->callbacks[type] = cb;
}
static GList * _global_menu_find_windows(GlobalMenuSocket * socket, char * dest_name){
	GList * window_list = NULL;
	GdkScreen * screen;
	GdkWindow * root = NULL;

	Window root_return;
	Window parent_return;
	Window * children_return;
	unsigned int nchildren_return;
	unsigned int i;
	gboolean connected = FALSE;
g_message("%s", __func__);
	g_return_val_if_fail( socket ,FALSE);
	g_return_val_if_fail( dest_name ,FALSE);
	
	screen = gdk_drawable_get_screen(socket->window);
	root = gdk_screen_get_root_window(screen);
	g_return_val_if_fail( screen , FALSE);	
	g_return_val_if_fail( root , FALSE);	

	gdk_error_trap_push();
	XQueryTree(GDK_DISPLAY_XDISPLAY(socket->display),
		GDK_WINDOW_XWINDOW(root),
		&root_return,
		&parent_return,
		&children_return,
		&nchildren_return);
	if(gdk_error_trap_pop()){
		g_warning("XQueryTree Failed");
		return FALSE;
	}

	g_return_val_if_fail( children_return , FALSE );

	for(i = 0; i < nchildren_return; i++){
		Atom type_return;
		Atom type_req = gdk_x11_get_xatom_by_name_for_display (socket->display, "UTF8_STRING");
		gint format_return;
		gulong nitems_return;
		gulong bytes_after_return;
		guchar * data;
		gint rt;
		gdk_error_trap_push();
		rt = XGetWindowProperty (GDK_DISPLAY_XDISPLAY (socket->display), children_return[i],
						  gdk_x11_get_xatom_by_name_for_display (socket->display, "_NET_WM_NAME"),
                          0, G_MAXLONG, False, type_req, &type_return,
                          &format_return, &nitems_return, &bytes_after_return,
                          &data);
		if(!gdk_error_trap_pop()){
			if(rt == Success && type_return == type_req){
				if(g_str_equal(dest_name, data)){
					g_message("Window found, Add to list");
	//				socket->dest_xid = children_return[i];
	//				connected = TRUE;
					window_list = g_list_append(window_list, (gpointer) children_return[i]);
				}
			}
		}else{
			g_warning("XGetWindowProperty Failed");
		}
	}
	XFree(children_return);
	return window_list;
}

static Window global_menu_socket_get_xid(GlobalMenuSocket * socket){
	return GDK_WINDOW_XWINDOW(socket->window);
}
static void global_menu_socket_send_to(GlobalMenuSocket * socket, Window xid, GlobalMenuNotify * message){
	XClientMessageEvent xclient;
	
	memset (&xclient, 0, sizeof (xclient));
	xclient.window = xid;
	xclient.type = ClientMessage;
	xclient.message_type = gdk_x11_get_xatom_by_name_for_display (socket->display, ATOM_STRING);
	xclient.format = 32;
	xclient.data.l[0] = gtk_get_current_event_time();
	xclient.data.l[1] = message->type;
	xclient.data.l[2] = GDK_WINDOW_XID(socket->window);/*Just use this socket's window id is Fine. message->param1;*/
	xclient.data.l[3] = message->param2;
	xclient.data.l[4] = message->param3;
	gdk_error_trap_push ();
	XSendEvent (GDK_DISPLAY_XDISPLAY(socket->display),
		  xid,
		  False, NoEventMask, (XEvent *)&xclient);
	gdk_display_sync (socket->display);
	gdk_error_trap_pop ();
	g_message("socket send");
}
static void global_menu_socket_send(GlobalMenuSocket * socket, GlobalMenuNotify * message){
	global_menu_socket_send_to(socket, socket->dest_xid, message);
}
static gboolean global_menu_socket_broadcast_by_name(GlobalMenuSocket * socket, char * dest_name, GlobalMenuNotify * notify){
	GList * window_list = NULL;
	GList * node = NULL;
	window_list = _global_menu_find_windows(socket, dest_name);
	for(node = g_list_first(window_list); node ; node = g_list_next(node)){
		g_message("%s: sending to %p", __func__, (gpointer) node->data);
		global_menu_socket_send_to(socket, node->data, notify);
	}
	g_list_free(window_list);
}
static gboolean global_menu_socket_connect_by_xid(GlobalMenuSocket * socket, Window xid){
	socket->dest_xid = xid; /*perhaps need to check whether the window is valid*/
	return TRUE;
}
static gboolean global_menu_socket_connect_by_name(GlobalMenuSocket * socket, gchar * dest_name){
	GList * window_list = NULL;
	gboolean rt = FALSE;
	window_list = _global_menu_find_windows(socket, dest_name);
	if(window_list){
		rt = global_menu_socket_connect_by_xid(socket, window_list->data); 
/*simply connect to the first server*/
		g_list_free(window_list);
	}
	return rt;
}
#endif
