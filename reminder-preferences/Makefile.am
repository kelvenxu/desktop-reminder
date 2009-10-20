SUBDIRS = 

bin_PROGRAMS = desktop-reminder-preferences
libexec_PROGRAMS =

common_defines = \
	-D_REENTRANT					\
	-DDBUS_API_SUBJECT_TO_CHANGE			\
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\"	\
	-DGCONF_PREFIX=\""/apps/desktop-reminder"\"		\
	-DDATADIR=\""$(datadir)"\"			\
	-DLIBEXECDIR=\""$(libexecdir)"\"		\
	-DBINDIR=\""$(bindir)"\"			\
	-DLOGO_PATH=DATADIR\"\"G_DIR_SEPARATOR_S\"lmplayer\"G_DIR_SEPARATOR_S\"lmplayer_logo.png\"	\
	$(DISABLE_DEPRECATED)

modules_flags = -export_dynamic -avoid-version -module -no-undefined

# Bacon message connection ltlibrary

# Lmplayer

#LMPLAYERMARSHALFILES = 						\
#	lmplayerobject-marshal.c lmplayerobject-marshal.h		\
#	lmplayerplaylist-marshal.c lmplayerplaylist-marshal.h		

#GLIB_GENMARSHAL=`pkg-config --variable=glib_genmarshal glib-2.0`
#BUILT_SOURCES = $(LMPLAYERMARSHALFILES)

#lmplayerobject-marshal.h: lmplayerobject-marshal.list Makefile
#	( $(GLIB_GENMARSHAL) --prefix=lmplayerobject_marshal $(srcdir)/lmplayerobject-marshal.list --header > lmplayerobject-marshal.h )
#lmplayerobject-marshal.c: lmplayerobject-marshal.h Makefile
#	( $(GLIB_GENMARSHAL) --prefix=lmplayerobject_marshal $(srcdir)/lmplayerobject-marshal.list --header --body > lmplayerobject-marshal.c )
#lmplayerplaylist-marshal.h: lmplayerplaylist-marshal.list Makefile
#	( $(GLIB_GENMARSHAL) --prefix=lmplayerplaylist_marshal $(srcdir)/lmplayerplaylist-marshal.list --header > lmplayerplaylist-marshal.h )
#lmplayerplaylist-marshal.c: lmplayerplaylist-marshal.h Makefile
#	( $(GLIB_GENMARSHAL) --prefix=lmplayerplaylist_marshal $(srcdir)/lmplayerplaylist-marshal.list --header --body > lmplayerplaylist-marshal.c )

desktop_reminder_preferences_SOURCES = 					\
														main.c

desktop_reminder_preferences_CPPFLAGS = \
	-I$(top_srcdir)/		\
	-I$(top_builddir)/data		\
	$(common_defines)		\
	$(AM_CPPFLAGS)

desktop_reminder_preferences_CFLAGS = \
	$(WARN_CFLAGS)			\
	$(EXTRA_GNOME_CFLAGS)		\
	$(DBUS_CFLAGS)			\
	$(NVTV_CFLAGS)			\
	$(MISSING_PLUGINS_CFLAGS) \
	$(AM_CFLAGS)

desktop_reminder_preferences_LDFLAGS = \
	$(AM_LDFLAGS)


desktop_reminder_preferences_LDADD =\
																		 $(EXTRA_GNOME_LIBS)

CLEANFILES =			\
	*.bak			\
	core*			\
	*.orig			\
	*~			\
	$(desktop_DATA)		\
	$(BUILT_SOURCES)

EXTRA_DIST = 				\
	lmplayerobject-marshal.list	\
	lmplayerplaylist-marshal.list
		
