#ifndef _GNOMENU_SMS_H_
#define _GNOMENU_SMS_H_


#pragma pack(push, 1)
typedef struct _GnomenuSMS {
	guchar action;
	union {
		guchar b[16];
		GdkNativeWindow w[1];
		gpointer p[1];
	};
} GnomenuSMS;
#pragma pack(pop)
typedef enum {
	MENUITEM_CLICKED,
	UPDATE_INTROSPECTION,
	INTROSPECTION_UPDATED,
} GnomenuSMSAction;

#endif



