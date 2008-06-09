#ifndef _GNOMENU_SMS_H_
#define _GNOMENU_SMS_H_


typedef struct _GnomenuSMS {
	guchar action;
	union {
		guchar b[18];
		Window w[1];
		gpointer p[1];
	};
} GnomenuSMS;
typedef enum {
	INVALIDATE_MENUBAR,
	MENUITEM_CLICKED,
} GnomenuSMSAction;

#endif



