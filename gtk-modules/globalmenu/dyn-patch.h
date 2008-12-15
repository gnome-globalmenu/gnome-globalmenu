#ifndef __DYN_PATCH_HELPER_H__
#define __DYN_PATCH_HELPER_H__
void dyn_patch_init(void);
void dyn_patch_queue_changed(GtkMenuBar * menubar, GtkWidget * widget);
void dyn_patch_set_menubar_r(GtkWidget * head, GtkMenuBar * menubar);
GtkMenuBar * dyn_patch_get_menubar(GtkWidget * widget);
void dyn_patch_set_menubar(GtkWidget * widget, GtkMenuBar * menubar);

#define DEFINE_FUNC(ret, t_ype, method, para) \
	ret ( * _old_ ## t_ype ## _ ## method ) para = NULL; \
	static ret ( _ ## t_ype ## _ ## method) para 

#define OVERRIDE( klass, t_ype, method ) \
	_old_ ## t_ype ## _ ## method = klass->method; \
	klass->method =  _ ## t_ype ## _ ## method;

#endif
