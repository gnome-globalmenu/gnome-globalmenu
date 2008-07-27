gboolean Ping(IPCCommand * command, gpointer data) {
	gchar * message = IPCParam(command, "message");
	g_message("Ping received from %s: %s", command->cid, message);
	IPCRet(command, g_strdup(message));
	return TRUE;
}
