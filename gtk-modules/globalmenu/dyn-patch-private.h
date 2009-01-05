#ifndef __DYN_PATCH_INTERNAL_H__
#define __DYN_PATCH_INTERNAL_H__
typedef void (*DynPatcherFunc)(GType type);
void dyn_patch_save_vfunc(const char * type, const char * name, gpointer vfunc);
gpointer dyn_patch_load_vfunc(const char * type, const char * name);
gpointer dyn_patch_hold_type(GType type);
void dyn_patch_release_type(GType type);
void dyn_patch_attach_menubar(GtkWindow * window, GtkMenuBar * menubar);
void dyn_patch_detach_menubar(GtkWindow * window, GtkMenuBar * menubar);

void dyn_patch_queue_changed(GtkMenuBar * menubar, GtkWidget * widget);
void dyn_patch_set_menubar_r(GtkWidget * head, GtkMenuBar * menubar);
GtkMenuBar * dyn_patch_get_menubar(GtkWidget * widget);
void dyn_patch_set_menubar(GtkWidget * widget, GtkMenuBar * menubar);

#define VFUNC_NAME(type, method) _ ## type ## _ ## method
#define VFUNC_TYPE(type, method) _ ## type ## _ ## method ## _t

#define DEFINE_FUNC(ret, type, method, para) \
	typedef ret ( * VFUNC_TYPE(type, method)) para; \
	static ret ( VFUNC_NAME(type, method)) para 

#define CHAINUP(type, method) ((VFUNC_TYPE(type, method)) dyn_patch_load_vfunc(#type, #method))

#define SAVE(klass, type, method) 

#define OVERRIDE_SAVE( klass, type, method ) \
	dyn_patch_save_vfunc(#type, #method, klass->method);\
	_OVERRIDE_NOCHECK(klass, type, method);

#define OVERRIDE( klass, type, method ) \
	if(klass->method == dyn_patch_load_vfunc(#type, #method)) { \
		_OVERRIDE_NOCHECK(klass, type, method); \
	}

#define _OVERRIDE_NOCHECK( klass, type, method ) \
		g_debug("override %s->%s_%s from %p to %p",  \
				G_OBJECT_CLASS_NAME(klass), \
				#type, #method, \
				klass->method, \
				VFUNC_NAME(type, method)); \
		klass->method = VFUNC_NAME(type, method); 

#define RESTORE( klass, type, method ) \
	if(klass->method == VFUNC_NAME(type, method)) { \
		g_debug("restore %s->%s_%s from %p to %p",  \
				G_OBJECT_CLASS_NAME(klass), \
				#type, #method, \
				klass->method, \
				dyn_patch_load_vfunc(#type, #method) \
				); \
		klass->method = dyn_patch_load_vfunc(#type, #method); \
	}

#endif
