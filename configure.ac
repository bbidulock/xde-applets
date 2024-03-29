#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([xde-applets],[0.12],[http://github.com/bbidulock/xde-applets],[xde-applets],[http://github.com/bbidulock/xde-applets/issues])
AC_REVISION([0.12])

# set better defaults
AC_PREFIX_DEFAULT([/usr])
if test "$prefix" = /usr -o "$prefix" = NONE; then
	test "$sysconfdir" = '${prefix}/etc' && sysconfdir=/etc
	test "$sharedstatedir" = '${prefix}/com' && sharedstatedir=/var
	test "$localstatedir" = '${prefix}/var' && localstatedir=/var
fi

AC_CONFIG_SRCDIR([src/xde-av-applet.c])
AC_CONFIG_HEADERS([autoconf.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([scripts])
AC_USE_SYSTEM_EXTENSIONS
AC_CANONICAL_TARGET

DATE='2022-01-29'
AC_SUBST([DATE])

MDOCDATE='January 29, 2022'
AC_SUBST([MDOCDATE])

BRANCH='0.12'
AC_SUBST([BRANCH])

# Initialize Automake
AM_INIT_AUTOMAKE([gnits 1.16 dist-lzip no-dist-gzip std-options -Wall color-tests silent-rules tar-pax])
AM_MAINTAINER_MODE
AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_SED
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AM_PROG_AR
AC_PROG_MKDIR_P
AC_ARG_VAR([LD], [Linker loader command])

AC_ARG_VAR([MARKDOWN],[markdown command. @<:@default=markdown@:>@])
AC_CHECK_PROGS([MARKDOWN],[markdown],[${am_missing_run}markdown],[$PATH])

AC_ARG_VAR([ASCIIDOC],[asciidoc command. @<:@default=asciidoctor@:>@])
AC_CHECK_PROGS([ASCIIDOC],[asciidoc asciidoctor],[${am_missing_run}asciidoc],[$PATH])

AC_ARG_VAR([ASCIIDOC_BACKEND],[asciidoc backend @<:@default=NONE@:>@])
if test -z "$ASCIIDOC_BACKEND"; then
	case "$ASCIIDOC" in
		asciidoc) ASCIIDOC_BACKEND="--backend=html4" ;;
		asciidoctor) ASCIIDOC_BACKEND="--backend=html5" ;;
		*) AC_MSG_WARN([ASCIIDOC $ASCIIDOC is not supported.]) ;;
	esac
fi

AC_ARG_VAR([ASCIIMAN],[asciidoc to manpage converter. @<:@default=a2x@:>@])
AC_CHECK_PROGS([ASCIIMAN],[a2x asciidoctor],[${am_missing_run}a2x],[$PATH])

AC_ARG_VAR([ASCIIMAN_FORMAT],[asciidoc manpage option @<:@default=NONE@:>@])
if test -z "$ASCIIMAN_FORMAT"; then
	case "$ASCIIMAN" in
		a2x) ASCIIMAN_FORMAT="--format=manpage" ;;
		asciidoctor) ASCIIMAN_FORMAT="--backend=manpage" ;;
		*) AC_MSG_WARN([ASCIIMAN $ASCIIMAN is not supported.]) ;;
	esac
fi

AC_ARG_VAR([LINKS],[links command. @<:@default=lynx@:>@])
AC_CHECK_PROGS([LINKS],[lynx links],[${am_missing_run}lynx],[$PATH])
if test "$LINKS" = "links"; then
	LINKS="links -codepage utf-8"
fi

AC_DISABLE_STATIC
LT_PATH_LD
LT_PREREQ([2.4.2])
LT_INIT([dlopen])
LT_LANG([C])
AC_SUBST([LIBTOOL_DEPS])

PKG_PROG_PKG_CONFIG([0.14.0])

# Checks for libraries.

# Checks for header files.
AC_CHECK_INCLUDES_DEFAULT
AC_PROG_EGREP

AC_PATH_X
AC_PATH_XTRA
AC_CHECK_HEADERS([fcntl.h locale.h stddef.h stdint.h stdlib.h string.h strings.h sys/ioctl.h sys/time.h syslog.h unistd.h langinfo.h])
AC_C_INLINE

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gethostname memmove select strcasecmp strchr strcspn strdup strerror strncasecmp strndup strpbrk strrchr strspn strstr strtol strtoul endpwent getcwd gettimeofday setlocale])

PKG_CHECK_MODULES([X11],[x11])
PKG_CHECK_MODULES([XEXT],[xext])
PKG_CHECK_MODULES([XAU],[xau])
PKG_CHECK_MODULES([XDMCP],[xdmcp])
PKG_CHECK_MODULES([ICE],[ice])
PKG_CHECK_MODULES([SMLIB],[sm])
PKG_CHECK_MODULES([XFT],[xft])
PKG_CHECK_MODULES([GDK],[gdk-pixbuf-xlib-2.0])
PKG_CHECK_MODULES([GLIB],[glib-2.0])
PKG_CHECK_MODULES([GOBJ],[gobject-2.0])
PKG_CHECK_MODULES([DBUS_GLIB],[dbus-glib-1])
PKG_CHECK_MODULES([GIO],[gio-2.0])
PKG_CHECK_MODULES([GIO_UNIX],[gio-unix-2.0])
PKG_CHECK_MODULES([GTK],[gtk+-2.0 >= 2.4.0])
PKG_CHECK_MODULES([UNIQUE],[unique-1.0])
PKG_CHECK_MODULES([WNCK],[libwnck-1.0])
PKG_CHECK_MODULES([CAIRO],[cairo])
PKG_CHECK_MODULES([SYSTEMD],[libsystemd])
PKG_CHECK_MODULES([PKGLIB],[packagekit-glib2])

AC_ARG_ENABLE([pulse],
	AS_HELP_STRING([--disable-pulse],[Disable Pulse support @<:@default=auto@:>@]))
if test "x$enable_pulse" != xno ; then
	PKG_CHECK_MODULES([PULSE],[libpulse],
		[AC_DEFINE([PULSE],[1], [Define to 1 to support Pulse library.])],
		[enable_pulse=no])
fi
AM_CONDITIONAL([PULSE],[test "x$enable_pulse" != xno])
AC_MSG_CHECKING([for Pulse support])
AC_MSG_RESULT([${enable_pulse:-yes}])
AM_CONDITIONAL([HAVE_PULSE],[test  "x$enable_pulse" != "xno"])

AC_ARG_ENABLE([gudev],
	AS_HELP_STRING([--disable-gudev],[Disable GUDEV support @<:@default=auto@:>@]))
if test "x$enable_gudev" != xno ; then
	PKG_CHECK_MODULES([GUDEV],[gudev-1.0],
		[AC_DEFINE([GUDEV],[1], [Define to 1 to support GUDEV library.])],
		[enable_gudev=no])
fi
AM_CONDITIONAL([GUDEV],[test "x$enable_gudev" != xno])
AC_MSG_CHECKING([for GUDEV support])
AC_MSG_RESULT([${enable_gudev:-yes}])
AM_CONDITIONAL([HAVE_GUDEV],[test  "x$enable_gudev" != "xno"])

AC_ARG_ENABLE([xf86vm],
	AS_HELP_STRING([--disable-xf86vm],[Disable XF86VM support @<:@default=auto@:>@]))
if test "x$enable_xf86vm" != xno ; then
	PKG_CHECK_MODULES([XF86VM],[xxf86vm],
		[AC_DEFINE([XF86VM],[1], [Define to 1 to support XF86VM extension.])],
		[enable_xf86vm=no])
fi
AM_CONDITIONAL([XRANDR],[test "x$enable_xf86vm" != xno])
AC_MSG_CHECKING([for XF86VM support])
AC_MSG_RESULT([${enable_xf86vm:-yes}])
AM_CONDITIONAL([HAVE_XF86VM],[test  "x$enable_xf86vm" != "xno"])

AC_ARG_ENABLE([xrandr],
	AS_HELP_STRING([--disable-xrandr],[Disable XRANDR support @<:@default=auto@:>@]))
if test "x$enable_xrandr" != xno ; then
	PKG_CHECK_MODULES([XRANDR],[xrandr],
		[AC_DEFINE([XRANDR],[1], [Define to 1 to support XRANDR extension.])],
		[enable_xrandr=no])
fi
AM_CONDITIONAL([XRANDR],[test "x$enable_xrandr" != xno])
AC_MSG_CHECKING([for XRANDR support])
AC_MSG_RESULT([${enable_xrandr:-yes}])
AM_CONDITIONAL([HAVE_XRANDR],[test  "x$enable_xrandr" != "xno"])

AC_ARG_ENABLE([xinerama],
	AS_HELP_STRING([--disable-xinerama],[Disable XINERAMA support @<:@default=auto@:>@]))
if test "x$enable_xinerama" != xno ; then
	PKG_CHECK_MODULES([XINERAMA],[xinerama],
		[AC_DEFINE([XINERAMA],[1], [Define to 1 to support XINERAMA extension.])],
		[enable_xinerama=no])
fi
AM_CONDITIONAL([XINERAMA],[test "x$enable_xinerama" != xno])
AC_MSG_CHECKING([for XINERAMA support])
AC_MSG_RESULT([${enable_xinerama:-yes}])
AM_CONDITIONAL([HAVE_XINERAMA],[test  "x$enable_xinerama" != "xno"])


AC_ARG_ENABLE([startup-notification],
	AS_HELP_STRING([--disable-startup-notification],[Disable startup notification support @<:@default=auto@:>@]))
if test "x$enable_startup_notification" != xno ; then
	PKG_CHECK_MODULES([SN],[libstartup-notification-1.0],
		[AC_DEFINE([STARTUP_NOTIFICATION],[1], [Define to 1 to support startup notification.])],
		[enable_startup_notification=no])
fi
AC_MSG_CHECKING([for startup notification support])
AC_MSG_RESULT([${enable_startup_notification:-yes}])

AC_ARG_ENABLE([notify],
	AS_HELP_STRING([--disable-notify],[Disable notification daemon notifications @<:@default=auto@:>@]))
if test "x$enable_notify" != xno ; then
	PKG_CHECK_MODULES([NOTIFY],[gio-2.0 gio-unix-2.0 glib-2.0 gdk-pixbuf-2.0 libnotify],
		[AC_DEFINE([DESKTOP_NOTIFICATIONS],[1],[Define to 1 to support desktop notifications.])],
		[enable_notify=no])
fi
AC_MSG_CHECKING([for notification-daemon support])
AC_MSG_RESULT([${enable_notify:-yes}])
AM_CONDITIONAL([HAVE_DESKTOP_NOTIFICATIONS],[test  "x$enable_notify" != "xno"])

AC_ARG_ENABLE([status-icon],
	AS_HELP_STRING([--disable-status-icon],[Disable status icon in system tray @<:@default=auto@:>@]))
if test "x$enable_status_icon" != xno ; then
	PKG_CHECK_MODULES([GTK2],[gtk+-2.0 >= 2.10],
		[AC_DEFINE([SYSTEM_TRAY_STATUS_ICON],[1],[Define to 1 to support system tray icon.])],
		[enable_status_icon=no])
fi
AC_MSG_CHECKING([for status-icon support])
AC_MSG_RESULT([${enable_status_icon:-yes}])
AM_CONDITIONAL([HAVE_SYSTEM_TRAY_STATUS_ICON],[test  "x$enable_status_icon" != "xno"])

AC_ARG_ENABLE([canberra],
	AS_HELP_STRING([--disable-canberra],[Disable canberra sound @<:@default=auto@:>@]))
if test "x$enable_canberra" != xno ; then
	PKG_CHECK_MODULES([SOUND],[libcanberra-gtk],
		[AC_DEFINE([CANBERRA_SOUND],[1],[Define to 1 to support libcanberra sound.])],
		[enable_canberra=no])
fi
AC_MSG_CHECKING([for libcanberra sound support])
AC_MSG_RESULT([${enable_canberra:-yes}])
AM_CONDITIONAL([HAVE_CANBERRA_SOUND],[test "x$enable_canberra" != "xno"])

AC_ARG_ENABLE([cpupower],
	AS_HELP_STRING([--disable-cpupower],[Disable cpupower @<:@default=auto@:>@]))
if test "x$enable_cpupower" != xno ; then
	AC_CHECK_HEADER([cpufreq.h],
		[AC_DEFINE([USE_CPUPOWER],[1],[Define to 1 to support cpupower.])],
		[enable_cpupower=no])
	AC_CHECK_LIB([cpupower], [cpufreq_get_driver],
		[AC_DEFINE([USE_CPUPOWER],[1],[Define to 1 to support cpupower.])],
		[enable_cpupower=no])
fi
AC_MSG_CHECKING([for cpupower support])
if test "x$enable_cpupower" != xno ; then
	CPU_CFLAGS=
	AC_SUBST([CPU_CFLAGS])
	CPU_LIBS=-lcpupower
	AC_SUBST([CPU_LIBS])
fi
AC_MSG_RESULT([${enable_cpupower:-yes}])
AM_CONDITIONAL([HAVE_CPUPOWER],[test "x$enable_cpupower" != "xno"])

AC_ARG_ENABLE([upower],
	AS_HELP_STRING([--disable-upower],[Disable upower @<:@default=auto@:>@]))
if test "x$enable_upower" != xno ; then
	PKG_CHECK_MODULES([UPOWER],[upower-glib],
		[AC_DEFINE([USE_UPOWER],[1],[Define to 1 to support libupower-glib.])],
		[enable_upower=no])
fi
AC_MSG_CHECKING([for libupower-glib support])
AC_MSG_RESULT([${enable_upower:-yes}])
AM_CONDITIONAL([HAVE_UPOWER_GLIB],[test "x$enable_upower" != "xno"])

AC_ARG_ENABLE([libdnet],
	AS_HELP_STRING([--disable-libdnet],[Disable libdnet @<:@default=auto@:>@]))
if test "x$enable_libdnet" != xno ; then
	AC_CHECK_HEADER([dnet.h],
		[AC_DEFINE([USE_LIBDNET],[1],[Define to 1 to support libdnet.])],
		[enable_libdnet=no])
	AC_CHECK_LIB([dnet], [eth_open],
		[AC_DEFINE([USE_LIBDNET],[1],[Define to 1 to support libdnet.])],
		[enable_libdnet=no])
fi
AC_MSG_CHECKING([for libdnet support])
if test "x$enable_libdnet" != xno ; then
	DNET_CFLAGS=
	AC_SUBST([DNET_CFLAGS])
	DNET_LIBS=-ldnet
	AC_SUBST([DNET_LIBS])
fi
AC_MSG_RESULT([${enable_libdnet:-yes}])
AM_CONDITIONAL([HAVE_LIBDNET],[test "x$enable_libdnet" != "xno"])

AC_ARG_ENABLE([sensors],
	AS_HELP_STRING([--disable-sensors],[Disable lm-sensors @<:@default=auto@:>@]))
if test "x$enable_sensors" != xno ; then
	AC_CHECK_HEADER([sensors/sensors.h],
		[AC_DEFINE([USE_LM_SENSORS],[1],[Define to 1 to support lm sensors.])],
		[enable_sensors=no])
	AC_CHECK_LIB([sensors], [sensors_init],
		[AC_DEFINE([USE_LM_SENSORS],[1],[Define to 1 to support lm sensors.])],
		[enable_sensors=no])
fi
AC_MSG_CHECKING([for lm-sensors support])
if test "x$enable_sensors" != xno ; then
	SENSOR_CFLAGS=
	AC_SUBST([SENSOR_CFLAGS])
	SENSOR_LIBS=-lsensors
	AC_SUBST([SENSOR_LIBS])
fi
AC_MSG_RESULT([${enable_sensors:-yes}])
AM_CONDITIONAL([HAVE_LM_SENSORS],[test "x$enable_sensors" != "xno"])

AC_ARG_ENABLE([debug],
	AS_HELP_STRING([--enable-debug],[Enable debugging support @<:@default=disabled@:>@]))
if test "x$enable_debug" = xyes ; then
	AC_DEFINE_UNQUOTED([DEBUG],[],[Define to enable debugging])
fi
AC_MSG_CHECKING([for debug support])
AC_MSG_RESULT([${enable_debug:-no}])

test "x$prefix" = xNONE && prefix="$ac_default_prefix"
sharedir="$datadir"
eval "sharedir=\"$sharedir\""
eval "sharedir=\"$sharedir\""
eval "sharedir=\"$sharedir\""
AC_SUBST([sharedir])

xdgconfdir="$sysconfdir"
eval "xdgconfdir=\"$xdgconfdir\""
eval "xdgconfdir=\"$xdgconfdir\""
eval "xdgconfdir=\"$xdgconfdir\""
AC_SUBST([xdgconfdir])

xdgdatadir="$prefix"
eval "xdgdatadir=\"$xdgdatadir\""
eval "xdgdatadir=\"$xdgdatadir\""
eval "xdgdatadir=\"$xdgdatadir\""
AC_SUBST([xdgdatadir])

autostartdir='$(xdgconfdir)/xdg/autostart'
AC_SUBST([autostartdir])

applicationsdir='$(xdgdatadir)/share/applications'
AC_SUBST([applicationsdir])

appdefaultsdir='$(sharedir)/X11/app-defaults'
AC_SUBST([appdefaultsdir])

statusicondir='$(sharedir)/icons/hicolor/scalable/status'
AC_SUBST([statusicondir])

deviceicondir='$(sharedir)/icons/hicolor/scalable/devices'
AC_SUBST([deviceicondir])

AC_CONFIG_FILES([Makefile
		 src/Makefile
		 man/Makefile
		 data/Makefile
		 data/icons/Makefile
		 data/autostart/Makefile
		 data/applications/Makefile
		 ])
AC_OUTPUT

# vim: set ft=config:
