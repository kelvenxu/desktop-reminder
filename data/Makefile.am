## Process this file with automake to produce Makefile.in

SUBDIRS=icons

stuffdir = $(pkgdatadir)


stuff_DATA=\
			 reminder-preferences.ui

desktopdir=$(datadir)/applications
desktop_in_files=desktop-reminder-preferences.desktop.in.in
desktop_DATA = $(desktop_in_files:.desktop.in.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST=$(stuff_DATA) $(desktop_in_files)

CLEANFILES = \
	*.bak *.uip core* *.orig *~ 	\
	$(desktop_DATA)			\
	$(generated_schema_in_files)	\
	$(schema_DATA)			
