
noinst_LTLIBRARIES = liblegacy-support.la

liblegacy_support_la_SOURCES= gmarkup-backport.h \
					   gmarkup-backport.c \
					   ghash-backport.c \
					   ghash-backport.h \
					   wnck-fake.c \
					   wnck-fake.h \
					   gtk-fake.h \
					   legacy-support.h

liblegacy_support_la_CPPFLAGS = -I$(top_srcdir) \
		-include $(CONFIG_HEADER) \
		$(WNCK_CFLAGS) \
		-DWNCK_I_KNOW_THIS_IS_UNSTABLE

