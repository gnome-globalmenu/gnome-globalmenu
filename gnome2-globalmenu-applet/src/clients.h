
void clients_entry_free(ClientEntry * entry);
ClientEntry * clients_add_dummy(gchar * title, Application * App);
ClientEntry * clients_add_local(gchar * title, GtkWidget* menubar, Application * App);
ClientEntry * clients_add_remote_by_stealing(WnckWindow * remoteMenuBarWindow, Application * App);
void clients_discover_all(Application * App);
void clients_remove_by_socket(GtkSocket * socket, Application * App);
ClientEntry * clients_find_by_socket(GtkSocket * socket, Application * App);
ClientEntry * clients_find_by_master(XWindowID master, Application * App);
void clients_set_active(ClientEntry * client, Application * App);
ClientEntry * clients_find_dummy(Application * App);

