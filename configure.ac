AC_PREREQ(2.52)

m4_define(desktop_reminder_version_major, 0)
m4_define(desktop_reminder_version_minor, 1)
m4_define(desktop_reminder_version_micro, 0)

AC_INIT([desktop-reminder],
        [desktop_reminder_version_major.desktop_reminder_version_minor.desktop_reminder_version_micro],
        [http://www.desktop-reminder.org])

AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h:config.in])

AM_INIT_AUTOMAKE([1.9 dist-bzip2 no-dist-gzip check-news subdir-objects])

dnl Add the languages which your application supports to po/LINGUAS
GETTEXT_PACKAGE=desktop-reminder
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, ["$GETTEXT_PACKAGE"], [Define to the Gettext package name])
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.35.0])

GNOME_DOC_INIT

AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
m4_ifdef([LT_OUTPUT], [LT_OUTPUT])

PKG_PROG_PKG_CONFIG

AC_CHECK_FUNCS(asprintf)
AC_CONFIG_LIBOBJ_DIR([lib])
AC_REPLACE_FUNCS([asprintf])

AC_PATH_PROG([GLIB_GENMARSHAL],[glib-genmarshal])
AC_PATH_PROG([GLIB_MKENUMS],[glib-mkenums])

# Requirements
GLIB_REQS=2.13.4
GTK_REQS=2.12.0
GNOMEUI_REQS=2.3.3
LIBGNOME_REQS=2.14.0
GNOMEICON_REQS=2.15.90
DBUS_REQS=0.61
VALA_REQS=0.1.6
TOTEM_PLPARSER_REQS=2.23.91
LIBSKIN_REQS=0.1.0

DESKTOP_REMINDER_VERSION_MAJOR=desktop_reminder_version_major
DESKTOP_REMINDER_VERSION_MINOR=desktop_reminder_version_minor
DESKTOP_REMINDER_VERSION_MICRO=desktop_reminder_version_micro
AC_SUBST(DESKTOP_REMINDER_VERSION_MAJOR)
AC_SUBST(DESKTOP_REMINDER_VERSION_MINOR)
AC_SUBST(DESKTOP_REMINDER_VERSION_MICRO)

dnl Check for GTK+ only build
AC_ARG_ENABLE(gtk,
	AC_HELP_STRING([--enable-gtk],[compile a gtk+ only version]),
	[case "${enableval}" in
	yes) ENABLE_GTK=yes ;;
	no) ENABLE_GTK=no ;;
	*) AC_MSG_ERROR(bad value ${enableval} for --disable-gtk) ;;
	esac],
	[ENABLE_GTK=no]) dnl Default value

dnl Seems that the order matters because libtool blows
dnl Also check if we're going to be using GTK+ only
if test x$ENABLE_GTK = "xno" ; then
# FIXME: use gmodule-no-export instead!
	PKG_CHECK_MODULES(EXTRA_GNOME, 
					  glib-2.0 >= $GLIB_REQS 
					  gtk+-2.0 >= $GTK_REQS 
					  gio-2.0 
					  gmodule-2.0 
					  $ISO_CODES)
	HAVE_GNOME=yes
else
# FIXME: use gmodule-no-export instead!
	PKG_CHECK_MODULES(EXTRA_GNOME, glib-2.0 >= $GLIB_REQS gtk+-2.0 >= $GTK_REQS gio-2.0 gnome-icon-theme >= $GNOMEICON_REQS gmodule-2.0 gconf-2.0)
	AC_DEFINE(USE_STABLE_LIBGNOMEUI, 1, [defined if we don't have GNOME])
	AC_DEFINE(HAVE_GTK_ONLY, 1, [defined if GNOME isn't used])
	HAVE_GNOME=no
fi

PKG_CHECK_MODULES(GTK, gtk+-2.0 gthread-2.0)
AC_SUBST(GTK_CFLAGS)
GTK_LIBS="$GTK_LIBS $EXTRA_BACKEND_LIBS"
AC_SUBST(GTK_LIBS)

AC_PATH_PROG(GCONFTOOL, gconftool-2)
AM_GCONF_SOURCE_2

GNOME_COMMON_INIT
GNOME_DEBUG_CHECK
GNOME_COMPILE_WARNINGS([maximum])
GNOME_CXX_WARNINGS
GNOME_MAINTAINER_MODE_DEFINES

# This macro expands DIR and assigns it to RET.
# If DIR is NONE, then it's replaced by DEFAULT.
# Based on AC_DEFINE_DIR
AC_DEFUN([DESKTOP_REMINDER_FULLPATH], [
  test "x$prefix" = xNONE && prefix="$ac_default_prefix"
  test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'
  ac_expand=[$]$1
  test "x$ac_expand" = xNONE && ac_expand="[$]$2"
  ac_expand=`eval echo [$]ac_expand`
  $3=`eval echo [$]ac_expand`
])

dnl FULL_LIBEXECDIR is used for X-GNOME-Bugzilla-ExtraInfoScript expansion
dnl in data/desktop_reminder.desktop.in.in.in
DESKTOP_REMINDER_FULLPATH(libexecdir, NONE, FULL_LIBEXECDIR)
AC_SUBST(FULL_LIBEXECDIR)

dnl debug
AC_ARG_ENABLE(debug,
		AC_HELP_STRING([--disable-debug],[disables compilation of debugging messages]),
	[case "${enableval}" in
	yes) ENABLE_DEBUG=yes ;;
	no)  ENABLE_DEBUG=no ;;
	*) AC_MSG_ERROR(bad value ${enableval} for --disable-debug) ;;
	esac],
[ENABLE_DEBUG=no]) dnl Default value
if test x$ENABLE_DEBUG = xyes; then
	AC_DEFINE(DESKTOP_REMINDER_DEBUG, 1, [Define if DEBUG statements should be compiled in])
fi

dnl run in source tree
AC_ARG_ENABLE(run-in-source-tree,
	      AC_HELP_STRING([--disable-run-in-source-tree],[disable search for UI files and plugins in local directory ]),
	[case "${enableval}" in
	yes) ENABLE_RUN_IN_SOURCE_TREE=yes ;;
	no)  ENABLE_RUN_IN_SOURCE_TREE=no ;;
	*) AC_MSG_ERROR(bad value ${enableval} for --disable-run-in-source-tree) ;;
	esac],
[ENABLE_RUN_IN_SOURCE_TREE=yes]) dnl Default value
if test x$ENABLE_RUN_IN_SOURCE_TREE = xyes; then
	AC_DEFINE(DESKTOP_REMINDER_RUN_IN_SOURCE_TREE, 1, [enable search for UI files and plugins in local directory])
fi

if test "$GCC" = "yes" ; then
	CFLAGS="\
	-Wall \
	-Wchar-subscripts -Wmissing-declarations -Wmissing-prototypes \
	-Wnested-externs -Wpointer-arith \
	-Wcast-align -Wsign-compare \
	-fno-strict-aliasing \
	$CFLAGS"
fi

AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])

AC_OUTPUT([
Makefile
src/Makefile
reminder-preferences/Makefile
data/Makefile
data/desktop-reminder-preferences.desktop.in
po/Makefile.in
data/icons/Makefile
data/icons/hicolor/Makefile
data/icons/hicolor/16x16/Makefile
data/icons/hicolor/16x16/apps/Makefile
data/icons/hicolor/22x22/Makefile
data/icons/hicolor/22x22/apps/Makefile
data/icons/hicolor/24x24/Makefile
data/icons/hicolor/24x24/apps/Makefile
data/icons/hicolor/32x32/Makefile
data/icons/hicolor/32x32/apps/Makefile
data/icons/hicolor/48x48/Makefile
data/icons/hicolor/48x48/apps/Makefile
data/icons/hicolor/scalable/Makefile
data/icons/hicolor/scalable/apps/Makefile
])


