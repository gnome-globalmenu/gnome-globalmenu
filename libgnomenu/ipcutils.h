
GdkNativeWindow ipc_find_server();
gpointer ipc_wait_for_property(GdkNativeWindow window, GdkAtom property_name, gboolean remove);
void ipc_send_client_message(GdkNativeWindow from, GdkNativeWindow to, GdkAtom message_type) ;
