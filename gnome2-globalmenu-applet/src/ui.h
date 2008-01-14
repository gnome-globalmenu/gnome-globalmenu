
struct _UICallbacks {
	GCallback label_area_action_cb;
	GCallback forward_action_cb; 
	GCallback backward_action_cb;
	GCallback holder_resize_cb;
	BonoboUIVerbFn menu_about_cb;
};
typedef struct _UICallbacks UICallbacks;
GtkEventBox * ui_create_label_area(struct _Application * App);
GtkEventBox * ui_create_event_box_with_icon(const gchar * stock_id);
void ui_repaint_all(struct _Application * App);
void ui_create_all(Application * App, UICallbacks * callbacks);
void ui_show_about(Application * App);
