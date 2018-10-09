/*****************************************************************************

 Copyright (c) 2010-2018  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2002-2009  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 *****************************************************************************/

/** @section Headers
  * @{ */

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif

#ifdef HAVE_CONFIG_H
#include "autoconf.h"
#endif

#undef STARTUP_NOTIFICATION

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>
#include <sys/utsname.h>

#include <assert.h>
#include <locale.h>
#include <langinfo.h>
#include <stdarg.h>
#include <strings.h>
#include <regex.h>
#include <wordexp.h>
#include <execinfo.h>
#include <math.h>
#include <dlfcn.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#ifdef XRANDR
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/randr.h>
#endif
#ifdef XINERAMA
#include <X11/extensions/Xinerama.h>
#endif
#ifdef STARTUP_NOTIFICATION
#define SN_API_NOT_YET_FROZEN
#include <libsn/sn.h>
#endif
#include <X11/SM/SMlib.h>
#include <gio/gio.h>
#include <gio/gdesktopappinfo.h>
#include <glib.h>
#include <glib-unix.h>
#include <gdk/gdkx.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <cairo.h>

#ifdef DESKTOP_NOTIFICATIONS
#include <libnotify/notify.h>
#endif

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

#include <pwd.h>
#include <fontconfig/fontconfig.h>
#include <pango/pangofc-fontmap.h>

#ifdef CANBERRA_SOUND
#include <canberra-gtk.h>
#endif

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

/** @} */

/** @section Preamble
  * @{ */

static const char *
_timestamp(void)
{
	static struct timeval tv = { 0, 0 };
	static char buf[BUFSIZ];
	double stamp;

	gettimeofday(&tv, NULL);
	stamp = (double)tv.tv_sec + (double)((double)tv.tv_usec/1000000.0);
	snprintf(buf, BUFSIZ-1, "%f", stamp);
	return buf;
}

#define XPRINTF(_args...) do { } while (0)

#define OPRINTF(_num, _args...) do { if (options.debug >= _num || options.output > _num) { \
		fprintf(stdout, NAME ": I: "); \
		fprintf(stdout, _args); fflush(stdout); } } while (0)

#define DPRINTF(_num, _args...) do { if (options.debug >= _num) { \
		fprintf(stderr, NAME ": D: [%s] %12s +%4d %s(): ", _timestamp(), __FILE__, __LINE__, __func__); \
		fprintf(stderr, _args); fflush(stderr); } } while (0)

#define EPRINTF(_args...) do { \
		fprintf(stderr, NAME ": E: [%s] %12s +%4d %s(): ", _timestamp(), __FILE__, __LINE__, __func__); \
		fprintf(stderr, _args); fflush(stderr);   } while (0)

#define WPRINTF(_args...) do { \
		fprintf(stderr, NAME ": W: [%s] %12s +%4d %s(): ", _timestamp(), __FILE__, __LINE__, __func__); \
		fprintf(stderr, _args); fflush(stderr);   } while (0)

#define PTRACE(_num) do { if (options.debug >= _num) { \
		fprintf(stderr, NAME ": T: [%s] %12s +%4d %s()\n", _timestamp(), __FILE__, __LINE__, __func__); \
		fflush(stderr); } } while (0)

void
dumpstack(const char *file, const int line, const char *func)
{
	void *buffer[32];
	int nptr;
	char **strings;
	int i;

	if ((nptr = backtrace(buffer, 32)) && (strings = backtrace_symbols(buffer, nptr)))
		for (i = 0; i < nptr; i++)
			fprintf(stderr, NAME ": E: %12s +%4d : %s() : \t%s\n", file, line, func, strings[i]);
}

#undef EXIT_SUCCESS
#undef EXIT_FAILURE
#undef EXIT_SYNTAXERR

#define EXIT_SUCCESS	0
#define EXIT_FAILURE	1
#define EXIT_SYNTAXERR	2

#define GTK_EVENT_STOP		TRUE
#define GTK_EVENT_PROPAGATE	FALSE

const char *program = NAME;

#define XA_PREFIX		"_XDE_UP_APPLET"
#define XA_SELECTION_NAME	XA_PREFIX "_S%d"
#define LOGO_NAME		"battery"

int saveArgc;
char **saveArgv;

#define RESNAME "xde-up-applet"
#define RESCLAS "XDE-UP-Applet"
#define RESTITL "XDE UPower Applet"

#define APPDFLT "/usr/share/X11/app-defaults/" RESCLAS

SmcConn smcConn = NULL;

int cmdArgc;
char **cmdArgv;

/** @} */

Atom _XA_XDE_ICON_THEME_NAME;	/* XXX */
Atom _XA_XDE_THEME_NAME;
Atom _XA_XDE_WM_CLASS;
Atom _XA_XDE_WM_CMDLINE;
Atom _XA_XDE_WM_COMMAND;
Atom _XA_XDE_WM_ETCDIR;
Atom _XA_XDE_WM_HOST;
Atom _XA_XDE_WM_HOSTNAME;
Atom _XA_XDE_WM_ICCCM_SUPPORT;
Atom _XA_XDE_WM_ICON;
Atom _XA_XDE_WM_ICONTHEME;	/* XXX */
Atom _XA_XDE_WM_INFO;
Atom _XA_XDE_WM_MENU;
Atom _XA_XDE_WM_NAME;
Atom _XA_XDE_WM_NETWM_SUPPORT;
Atom _XA_XDE_WM_PID;
Atom _XA_XDE_WM_PRVDIR;
Atom _XA_XDE_WM_RCFILE;
Atom _XA_XDE_WM_REDIR_SUPPORT;
Atom _XA_XDE_WM_STYLE;
Atom _XA_XDE_WM_STYLENAME;
Atom _XA_XDE_WM_SYSDIR;
Atom _XA_XDE_WM_THEME;
Atom _XA_XDE_WM_THEMEFILE;
Atom _XA_XDE_WM_USRDIR;
Atom _XA_XDE_WM_VERSION;

Atom _XA_GTK_READ_RCFILES;
Atom _XA_MANAGER;

Atom _XA_XDE_UP_APPLET_REFRESH;
Atom _XA_XDE_UP_APPLET_RESTART;
Atom _XA_XDE_UP_APPLET_POPMENU;
Atom _XA_XDE_UP_APPLET_REQUEST;

#define CA_CONTEXT_ID	55

typedef enum {
	CaEventWindowManager = CA_CONTEXT_ID,
	CaEventWorkspaceChange,
	CaEventDesktopChange,
	CaEventWindowChange,
	CaEventLockScreen,
	CaEventPowerChanged,
	CaEventSleepSuspend,
	CaEventBatteryLevel,
} CaEventId;

typedef struct {
	int index;
	GdkDisplay *disp;
	GdkScreen *scrn;
	GdkWindow *root;
	WnckScreen *wnck;
	char *theme;
	char *itheme;
	Window selwin;
	Window owner;
	Atom atom;
	char *wmname;
	Bool goodwm;
	GdkPixbuf *icon;
	cairo_t *cr;	    /* for drawing on dockapp window */
	cairo_t *cr2;	    /* for drawing on pixmap */
	GdkWindow *iwin;
	GtkStatusIcon *status;
	GdkPixmap *pmap;
} XdeScreen;

typedef enum {
	CommandDefault,	    /* just generate WM root menu */
	CommandMenugen,    /* just generate WM root menu */
	CommandMonitor,	    /* run a new instance with monitoring */
	CommandQuit,	    /* ask running instance to quit */
	CommandPopMenu,	    /* ask running instance to pop menu */
	CommandRefresh,	    /* ask running instance to refresh menu */
	CommandRestart,	    /* ask running instance to restart */
	CommandReplace,	    /* replace a running instance */
	CommandHelp,	    /* print usage info and exit */
	CommandVersion,	    /* print version info and exit */
	CommandCopying,	    /* print copying info and exit */
} Command;

typedef struct {
	int debug;
	int output;
	Command command;
	char *display;
	int screen;
	char *clientId;
	char *saveFile;
	Bool tray;
	Bool dock;
	Bool dieonerr;
} Options;

Options options = {
	.debug = 0,
	.output = 1,
	.command = CommandDefault,
	.display = NULL,
	.screen = 0,
	.clientId = NULL,
	.saveFile = NULL,
	.tray = True,
	.dock = True,
	.dieonerr = False,
};

Options defaults = {
	.debug = 0,
	.output = 1,
	.command = CommandDefault,
	.display = NULL,
	.screen = 0,
	.clientId = NULL,
	.saveFile = NULL,
	.tray = True,
	.dock = True,
	.dieonerr = False,
};

XdeScreen *screens = NULL;

char *xdg_data_home = NULL;
char *xdg_data_dirs = NULL;
char *xdg_data_path = NULL;
char *xdg_data_last = NULL;

static inline char *
xdg_find_str(char *s, char *b)
{
	for (s--; s > b && *s != '\0'; s--) ;
	if (s > b)
		s++;
	return (s);
}

char *xdg_config_home = NULL;
char *xdg_config_dirs = NULL;
char *xdg_config_path = NULL;
char *xdg_config_last = NULL;

GMainLoop *loop = NULL;

static gboolean
on_button_press(GtkStatusIcon *icon, GdkEvent *event, gpointer user_data)
{
	XdeScreen *xscr = user_data;
	GdkEventButton *ev;

	(void) xscr;
	ev = (typeof(ev)) event;
	if (ev->button != 1)
		return GTK_EVENT_PROPAGATE;
	/* FIXME: do something */
	return GTK_EVENT_STOP;
}

void
on_services_selected(GtkMenuItem *item, gpointer user_data)
{
}

void
on_ssh_selected(GtkMenuItem *item, gpointer user_data)
{
}

void
on_vnc_selected(GtkMenuItem *item, gpointer user_data)
{
}

void
on_about_selected(GtkMenuItem *item, gpointer user_data)
{
	gchar *authors[] = { "Brian F. G. Bidulock <bidulock@openss7.org>", NULL };
	gtk_show_about_dialog(NULL,
			      "authors", authors,
			      "comments", "A Upower system tray icon.",
			      "copyright", "Copyright (c) 2013, 2014, 2015, 2016, 2017, 2018  OpenSS7 Corporation",
			      "license", "Do what thou wilt shall be the whole of the law.\n\n-- Aleister Crowley",
			      "logo-icon-name", LOGO_NAME,
			      "program-name", NAME,
			      "version", VERSION,
			      "website", "http://www.unexicon.com/",
			      "website-label", "Unexicon - Linux spun for telecom",
			      NULL);
	return;
}

static void
applet_refresh(XdeScreen *xscr)
{
}

/** @brief restart the applet
  *
  * We restart the applet by executing ourselves with the same arguments that were
  * provided in the command that started us.  However, if we are running under
  * session management with restart hint SmRestartImmediately, the session
  * manager will restart us if we simply exit.
  */
static void
applet_restart(void)
{
	/* asked to restart the applet (as though we were re-executed) */
	char **argv;
	int i;

	if (smcConn) {
		/* When running under a session manager, simply exit and the session
		   manager will restart us immediately. */
		exit(EXIT_SUCCESS);
	}

	argv = calloc(saveArgc + 1, sizeof(*argv));
	for (i = 0; i < saveArgc; i++)
		argv[i] = saveArgv[i];

	DPRINTF(1, "%s: restarting the applet\n", NAME);
	if (execvp(argv[0], argv) == -1)
		EPRINTF("%s: %s\n", argv[0], strerror(errno));
	return;
}

void
on_redo_selected(GtkMenuItem *item, gpointer user_data)
{
	applet_restart();
}

void
on_quit_selected(GtkMenuItem *item, gpointer user_data)
{
	gtk_main_quit();
}

static void
on_popup_menu(GtkStatusIcon *icon, guint button, guint time, gpointer user_data)
{
	XdeScreen *xscr = user_data;
	GtkWidget *menu, *item, *imag;

	menu = gtk_menu_new();

	item = gtk_image_menu_item_new_with_label("Services...");
	imag = gtk_image_new_from_icon_name("applications-system", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), imag);
	g_signal_connect(item, "activate", G_CALLBACK(on_services_selected), xscr);
	gtk_widget_show(item);
	gtk_menu_append(menu, item);

	item = gtk_image_menu_item_new_with_label("SSH...");
	imag = gtk_image_new_from_icon_name("system-file-manager", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), imag);
	g_signal_connect(item, "activate", G_CALLBACK(on_ssh_selected), xscr);
	gtk_widget_show(item);
	gtk_menu_append(menu, item);

	item = gtk_image_menu_item_new_with_label("VNC...");
	imag = gtk_image_new_from_icon_name("system-file-manager", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), imag);
	g_signal_connect(item, "activate", G_CALLBACK(on_vnc_selected), xscr);
	gtk_widget_show(item);
	gtk_menu_append(menu, item);

	item = gtk_separator_menu_item_new();
	gtk_widget_show(item);
	gtk_menu_append(menu, item);

	item = gtk_image_menu_item_new_from_stock("gtk-about", NULL);
	g_signal_connect(item, "activate", G_CALLBACK(on_about_selected), xscr);
	gtk_widget_show(item);
	gtk_menu_append(menu, item);

	item = gtk_image_menu_item_new_from_stock("gtk-redo", NULL);
	g_signal_connect(item, "activate", G_CALLBACK(on_redo_selected), xscr);
	gtk_widget_show(item);
	gtk_menu_append(menu, item);

	item = gtk_image_menu_item_new_from_stock("gtk-quit", NULL);
	g_signal_connect(item, "activate", G_CALLBACK(on_quit_selected), xscr);
	gtk_widget_show(item);
	gtk_menu_append(menu, item);

	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, gtk_status_icon_position_menu, icon, button, time);
	return;
}

static void
init_statusicon(XdeScreen *xscr)
{
	GtkStatusIcon *icon;

	icon = gtk_status_icon_new_from_icon_name(LOGO_NAME);
	gtk_status_icon_set_tooltip_text(icon, "Click for menu...");
	gtk_status_icon_set_visible(icon, TRUE);
	g_signal_connect(icon, "button_press_event",
			G_CALLBACK(on_button_press), xscr);
	g_signal_connect(icon, "popup_menu",
			G_CALLBACK(on_popup_menu), xscr);
	xscr->status = icon;
}

static GdkFilterReturn
dockapp_handler(GdkXEvent * xevent, GdkEvent * event, gpointer data)
{
	XdeScreen *xscr = data;
	XEvent *xev = xevent;

	DPRINTF(1, "Event of type %d(%d)\n", event->type, xev->type);
	if (xev->type == Expose) {
		GdkRectangle rect =
		    { xev->xexpose.x, xev->xexpose.y, xev->xexpose.width, xev->xexpose.height };
		gdk_window_clear_area(xscr->iwin, xev->xexpose.x, xev->xexpose.y,
				xev->xexpose.width, xev->xexpose.height);
		gdk_cairo_rectangle(xscr->cr, &rect);
		cairo_clip(xscr->cr);
		cairo_paint(xscr->cr);
		gdk_cairo_reset_clip(xscr->cr, GDK_DRAWABLE(xscr->iwin));
	}
	return GDK_FILTER_CONTINUE;
}

static void
init_dockapp(XdeScreen *xscr)
{
	GdkWindowAttr attrs = { NULL, };
	XWMHints wmhints = { 0, };
	XSizeHints sizehints = { 0, };
	int attrs_mask = 0;
	Window icon;

	attrs.event_mask = GDK_ALL_EVENTS_MASK;
	attrs.event_mask |= GDK_EXPOSURE_MASK;
	attrs.event_mask |= GDK_POINTER_MOTION_HINT_MASK;
	attrs.event_mask |= GDK_BUTTON_PRESS_MASK;
	attrs.event_mask |= GDK_BUTTON_RELEASE_MASK;
	attrs.event_mask |= GDK_KEY_PRESS_MASK;
	attrs.event_mask |= GDK_KEY_RELEASE_MASK;
	attrs.event_mask |= GDK_ENTER_NOTIFY_MASK;
	attrs.event_mask |= GDK_LEAVE_NOTIFY_MASK;
	attrs.event_mask |= GDK_FOCUS_CHANGE_MASK;
	attrs.event_mask |= GDK_STRUCTURE_MASK;
	attrs.event_mask |= GDK_SUBSTRUCTURE_MASK;
	attrs.event_mask |= GDK_SCROLL_MASK;
	/* might want some more */
	attrs.width = 64;
	attrs.height = 64;
	attrs.wclass = GDK_INPUT_OUTPUT;
	attrs.window_type = GDK_WINDOW_TOPLEVEL;

	attrs.title = g_strdup_printf("Blah %s", "Blah");
	attrs_mask |= GDK_WA_TITLE;
#if 0
	attrs.x = 0;
	attrs_mask |= GDK_WA_X;
	attrs.y = 0;
	attrs_mask |= GDK_WA_Y;
	attrs.cursor = /* basic cursor */ ;
	attrs_mask |= GDK_WA_CURSOR;
	attrs.override_redirect = FALSE;
	attrs_mask |= GDK_WA_NOREDIR;
#endif
	attrs.visual = gdk_visual_get_system();
	attrs_mask |= GDK_WA_VISUAL;
	attrs.colormap = gdk_colormap_get_system();
	attrs_mask |= GDK_WA_COLORMAP;
	attrs.wmclass_name = "xdeavapplet";
	attrs.wmclass_class = "DockApp";
	attrs_mask |= GDK_WA_WMCLASS;
#if 0
	attrs.type_hint = GDK_WINDOW_TYPE_HINT_UTILITY;
	attrs_mask |= GDK_WA_TYPE_HINT;
#endif

	xscr->iwin = gdk_window_new(NULL, &attrs, attrs_mask);
	gdk_window_set_back_pixmap(xscr->iwin, NULL, TRUE);
	gdk_window_add_filter(xscr->iwin, dockapp_handler, xscr);

	/* set the window's icon window to itself */
	gdk_window_set_icon(xscr->iwin, xscr->iwin, NULL, NULL);

	/* largely for when the WM does not support dock apps */
#if 0
	gdk_window_set_type_hint(xscr->iwin, GDK_WINDOW_TYPE_HINT_DOCK);
#else
	gdk_window_set_decorations(xscr->iwin, FALSE);
	gdk_window_set_skip_taskbar_hint(xscr->iwin, TRUE);
	gdk_window_set_skip_pager_hint(xscr->iwin, TRUE);
#endif
	icon = GDK_WINDOW_XID(xscr->iwin);

	/* make this user specified size so WM does not mess with it */
	sizehints.flags = USSize;
	sizehints.width = 64;
	sizehints.height = 64;
	XSetWMNormalHints(GDK_WINDOW_XDISPLAY(xscr->iwin), icon, &sizehints);

	/* set the window to start in the withdrawn state */
	wmhints.flags = 0;
	wmhints.input = True;
	wmhints.flags |= InputHint;
	wmhints.initial_state = WithdrawnState;
	wmhints.flags |= StateHint;
	wmhints.icon_window = icon;
	wmhints.flags |= IconWindowHint;
	wmhints.icon_x = 0;
	wmhints.icon_y = 0;
	wmhints.flags |= IconPositionHint;
	wmhints.window_group = icon;
	wmhints.flags |= WindowGroupHint;

	{
		Window t, p, dummy1, *dummy2;
		unsigned int dummy3;
		Display *dpy = GDK_WINDOW_XDISPLAY(xscr->iwin);

		/* NOTE: this has to be done this way for GDK2, otherwise, the window is
		   mapped to the window manager with an initial state of NormalState.
		   So, we reparent the window under a temporary window (from root) before 
		   gdk_window_show() and then set the proper WMHints and then reparent it 
		   back to root in the mapped state. */
		XQueryTree(dpy, icon, &dummy1, &p, &dummy2, &dummy3);
		if (dummy2)
			XFree(dummy2);
		t = XCreateSimpleWindow(dpy, p, 0, 0, 1, 1, 0, 0, 0);
		XReparentWindow(dpy, icon, t, 0, 0);
		gdk_window_show(xscr->iwin);
		XSetWMHints(dpy, icon, &wmhints);
		XReparentWindow(dpy, icon, p, 0, 0);
		XSetWindowBackgroundPixmap(dpy, icon, ParentRelative);
		XDestroyWindow(dpy, t);
	}

	GtkIconTheme *itheme = gtk_icon_theme_get_default();

	xscr->icon = gtk_icon_theme_load_icon(itheme, LOGO_NAME, 48,
					      GTK_ICON_LOOKUP_USE_BUILTIN | GTK_ICON_LOOKUP_FORCE_SIZE, NULL);
	if (!xscr->icon) {
		EPRINTF("could not get icon!\n");
		exit(EXIT_FAILURE);
	}

	xscr->pmap = gdk_pixmap_new(NULL, 64, 64, 32);
	GdkVisual *visual = gdk_visual_get_best_with_depth(32);
	GdkColormap *cmap = gdk_colormap_new(visual, FALSE);
	gdk_drawable_set_colormap(GDK_DRAWABLE(xscr->pmap), cmap);

	xscr->cr2 = gdk_cairo_create(GDK_DRAWABLE(xscr->pmap));
	gdk_cairo_set_source_pixbuf(xscr->cr2, xscr->icon, 4.0, 8.0);
	cairo_set_operator(xscr->cr2, CAIRO_OPERATOR_CLEAR);
	cairo_paint(xscr->cr2);
	cairo_set_operator(xscr->cr2, CAIRO_OPERATOR_SOURCE);
	cairo_paint(xscr->cr2);

	xscr->cr = gdk_cairo_create(GDK_DRAWABLE(xscr->iwin));
	gdk_cairo_set_source_pixmap(xscr->cr, xscr->pmap, 0.0, 0.0);
	cairo_paint(xscr->cr);
}

ca_context *
get_default_ca_context(void)
{
	GdkDisplay *disp = gdk_display_get_default();
	GdkScreen *scrn = gdk_display_get_default_screen(disp);
	ca_context *ca = ca_gtk_context_get_for_screen(scrn);

	return (ca);
}

GDBusProxy *up_manager = NULL;
GList *up_devices = NULL;
gchar *up_critical = NULL;
gboolean battery_low = FALSE;


typedef struct {
	char *path;
	gboolean display;
	GDBusProxy *proxy;
	NotifyNotification *notify;
} XdeDevice;

void
xde_device_destroy(gpointer data)
{
	XdeDevice *xdev = data;

	g_free(xdev->path);
	g_object_unref(G_OBJECT(xdev->proxy));
	g_object_unref(G_OBJECT(xdev->notify));
	free(xdev);
}

void
xde_manager_dump(GDBusProxy *proxy)
{
	GVariant *prop;

	if ((prop = g_dbus_proxy_get_cached_property(proxy, "DaemonVersion"))) {
		DPRINTF(1, "DaemonVersion: \"%s\"\n", g_variant_get_string(prop, NULL));
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "OnBattery"))) {
		DPRINTF(1, "OnBattery: %s\n", g_variant_get_boolean(prop) ? "true" : "false");
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "LidIsClosed"))) {
		DPRINTF(1, "LidIsClosed: %s\n", g_variant_get_boolean(prop) ? "true" : "false");
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "LidIsPresent"))) {
		DPRINTF(1, "LidIsPresent: %s\n", g_variant_get_boolean(prop) ? "true" : "false");
		g_variant_unref(prop);
	}
}

void
update_display_icon(XdeDevice *xdev, const gchar *icon)
{
	GtkIconTheme *theme;
	GdkPixbuf *pixbuf;
	GdkDisplay *disp;
	XdeScreen *xscr;
	int s, nscr;
	gchar *name;

	if (!icon || !xdev->display || !(name = g_strdup(icon)))
		return;
	if (1) {
		gchar *p;

		if ((p = strstr(name, "-symbolic")))
			*p = '\0';
	}
	/* using a 48x48 display icon, we can put 1x4 16x16 status icons along the right */
	theme = gtk_icon_theme_get_default();
	pixbuf = gtk_icon_theme_load_icon(theme, name, 48,
					  GTK_ICON_LOOKUP_USE_BUILTIN | GTK_ICON_LOOKUP_FORCE_SIZE, NULL);
	if (pixbuf) {
		disp = gdk_display_get_default();
		nscr = gdk_display_get_n_screens(disp);
		for (s = 0, xscr = screens; s < nscr; s++, xscr++) {
			if (xscr->status)
				gtk_status_icon_set_from_icon_name(xscr->status, name);
			if (xscr->cr2) {
				gdk_cairo_set_source_pixbuf(xscr->cr2, pixbuf, 4.0, 8.0);
				cairo_set_operator(xscr->cr2, CAIRO_OPERATOR_CLEAR);
				cairo_paint(xscr->cr2);
				cairo_set_operator(xscr->cr2, CAIRO_OPERATOR_SOURCE);
				cairo_paint(xscr->cr2);
			}
			if (xscr->cr) {
				gdk_cairo_set_source_pixmap(xscr->cr, xscr->pmap, 0.0, 0.0);
				gdk_window_clear(xscr->iwin);
				cairo_paint(xscr->cr);
			}
		}
	}
	g_free(name);
}

const char *
show_type(uint32_t type)
{
	static char buf[64];

	switch (type) {
	case 0:
		return ("Unknown(0)");
	case 1:
		return ("Line Power(1)");
	case 2:
		return ("Battery(2)");
	case 3:
		return ("Ups(3)");
	case 4:
		return ("Monitor(4)");
	case 5:
		return ("Mouse(5)");
	case 6:
		return ("Keyboard(6)");
	case 7:
		return ("Pda(7)");
	case 8:
		return ("Phone(8)");
	default:
		snprintf(buf, sizeof(buf), "Unknown(%u)", type);
		return (buf);
	}
}

const char *
show_state(uint32_t state)
{
	static char buf[64];

	switch (state) {
	case 0:
		return ("Unknown(0)");
	case 1:
		return ("Charging(1)");
	case 2:
		return ("Discharging(2)");
	case 3:
		return ("Empty(3)");
	case 4:
		return ("Fully charged(4)");
	case 5:
		return ("Pending charge(5)");
	case 6:
		return ("Pending discharge(6)");
	default:
		snprintf(buf, sizeof(buf), "Unknown(%u)", state);
		return (buf);
	}
}

const char *
show_technology(uint32_t tech)
{
	static char buf[64];

	switch (tech) {
	case 0:
		return ("Unknown(0)");
	case 1:
		return ("Lithium ion(1)");
	case 2:
		return ("Lithium polymer(2)");
	case 3:
		return ("Lithium iron phosphate(3)");
	case 4:
		return ("Lead acid(4)");
	case 5:
		return ("Nickel cadmium(5)");
	case 6:
		return ("Nickel metal hydride(6)");
	default:
		snprintf(buf, sizeof(buf), "Unknown(%u)", tech);
		return (buf);
	}
}

const char *
show_warning(uint32_t warn)
{
	static char buf[64];

	switch (warn) {
	case 0:
		return ("Unknown(0)");
	case 1:
		return ("None(1)");
	case 2:
		return ("Discharging(2)");
	case 3:
		return ("Low(3)");
	case 4:
		return ("Critical(4)");
	case 5:
		return ("Action(5)");
	default:
		snprintf(buf, sizeof(buf), "Unknown(%u)", warn);
		return (buf);
	}
}

const char *
show_level(uint32_t level)
{
	static char buf[64];

	switch (level) {
	case 0:
		return ("Unknown(0)");
	case 1:
		return ("None(1)");
	case 3:
		return ("Low(3)");
	case 4:
		return ("Critical(4)");
	case 6:
		return ("Normal(6)");
	case 7:
		return ("High(7)");
	case 8:
		return ("Full(8)");
	default:
		snprintf(buf, sizeof(buf), "Unknown(%u)", level);
		return (buf);
	}
}


void
xde_device_dump(GDBusProxy *proxy)
{
	GVariant *prop;

	if ((prop = g_dbus_proxy_get_cached_property(proxy, "NativePath"))) {
		DPRINTF(1, "NativePath: \"%s\"\n", g_variant_get_string(prop, NULL));
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "Vendor"))) {
		DPRINTF(1, "Vendor: \"%s\"\n", g_variant_get_string(prop, NULL));
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "Model"))) {
		DPRINTF(1, "Model: \"%s\"\n", g_variant_get_string(prop, NULL));
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "Serial"))) {
		DPRINTF(1, "Serial: \"%s\"\n", g_variant_get_string(prop, NULL));
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "UpdateTime"))) {
		DPRINTF(1, "UpdateTime: %lu sec\n", g_variant_get_uint64(prop));
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "Type"))) {
		DPRINTF(1, "Type: %s\n", show_type(g_variant_get_uint32(prop)));
		/* Unknown(0), Line Power(1), Battery(2), Ups(3), Monitor(4), Mouse(5), Keyboard(6), Pda(7), Phone(8) */
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "PowerSupply"))) {
		DPRINTF(1, "PowerSupply: %s\n", g_variant_get_boolean(prop) ? "true" : "false");
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "HasHistory"))) {
		DPRINTF(1, "HasHistory: %s\n", g_variant_get_boolean(prop) ? "true" : "false");
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "HasStatistics"))) {
		DPRINTF(1, "HasStatistics: %s\n", g_variant_get_boolean(prop) ? "true" : "false");
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "Online"))) {
		DPRINTF(1, "Online: %s\n", g_variant_get_boolean(prop) ? "true" : "false");
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "Energy"))) {
		DPRINTF(1, "Energy: %f Wh\n", g_variant_get_double(prop));
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "EnergyEmpty"))) {
		DPRINTF(1, "EnergyEmpty: %f Wh\n", g_variant_get_double(prop));
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "EnergyFull"))) {
		DPRINTF(1, "EnergyFull: %f Wh\n", g_variant_get_double(prop));
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "EnergyFUllDesign"))) {
		DPRINTF(1, "EnergyFullDesign: %f Wh\n", g_variant_get_double(prop));
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "EnergyRate"))) {
		DPRINTF(1, "EnergyRate: %f W\n", g_variant_get_double(prop));
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "Voltage"))) {
		DPRINTF(1, "Voltage: %f V\n", g_variant_get_double(prop));
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "Luminosity"))) {
		DPRINTF(1, "Luminosity: %f\n", g_variant_get_double(prop));
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "TimeToEmpty"))) {
		DPRINTF(1, "TimeToEmpty: %ld sec\n", g_variant_get_int64(prop));
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "TimeToFull"))) {
		DPRINTF(1, "TimeToFull: %ld sec\n", g_variant_get_int64(prop));
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "Percentage"))) {
		DPRINTF(1, "Percentage: %f%%\n", g_variant_get_double(prop));
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "Temperature"))) {
		DPRINTF(1, "Temperature: %f °C\n", g_variant_get_double(prop));
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "IsPresent"))) {
		DPRINTF(1, "IsPresent: %s\n", g_variant_get_boolean(prop) ? "true" : "false");
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "State"))) {
		DPRINTF(1, "State: %s\n", show_state(g_variant_get_uint32(prop)));
		/* Unknown(0), Charging(1), Discharging(2), Empty(3), Fully charged(4), Pending charge(5), Pending discharge(6) */
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "IsRechargeable"))) {
		DPRINTF(1, "IsRechargeable: %s\n", g_variant_get_boolean(prop) ? "true" : "false");
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "Capacity"))) {
		DPRINTF(1, "Capacity: %f%%\n", g_variant_get_double(prop));
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "Technology"))) {
		DPRINTF(1, "Technology: %s\n", show_technology(g_variant_get_uint32(prop)));
		/* Unknown(0), Lithium ion(1), Lithium polymer(2), Lithium iron phosphate(3), Lead acid(4), Nickel cadmium(5), Nickel metal hydride(6) */
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "WarningLevel"))) {
		DPRINTF(1, "WarningLevel: %s\n", show_warning(g_variant_get_uint32(prop)));
		/* Unknown(0), None(1), Discharging(2) (UPS only), Low(3), Critical(4), Action(5) */
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "BatteryLevel"))) {
		DPRINTF(1, "BatteryLevel: %s\n", show_level(g_variant_get_uint32(prop)));
		/* Unknown(0), None(1), Low(3), Critical(4), Normal(6), High(7), Full(8) */
		g_variant_unref(prop);
	}
	if ((prop = g_dbus_proxy_get_cached_property(proxy, "IconName"))) {
		DPRINTF(1, "IconName: \"%s\"\n", g_variant_get_string(prop, NULL));
		g_variant_unref(prop);
	}
}

void
xde_device_warn(XdeDevice *xdev, guint32 level)
{
	ca_context *ca = get_default_ca_context();
	ca_proplist *pl;

	notify_notification_close(xdev->notify, NULL);
	ca_context_cancel(ca, CaEventBatteryLevel);
	ca_proplist_create(&pl);

	switch (level) {
	default:
	case 0:		/* Unknown(0) */
		if (xdev->display) {
			battery_low = FALSE;
		}
		break;
	case 1:		/* None(1) */
		if (xdev->display) {
			battery_low = FALSE;
		}
		break;
	case 2:		/* Discharging(2) */
		if (xdev->display) {
			battery_low = FALSE;
		}
		break;
	case 3:		/* Low(3) */
		if (xdev->display) {
			battery_low = TRUE;
			ca_proplist_sets(pl, CA_PROP_EVENT_ID, "battery-low");
			notify_notification_update(xdev->notify,
					"Warning",
					"The battery level is low.",
					"battery-low");
			notify_notification_set_timeout(xdev->notify, 5);
			notify_notification_show(xdev->notify, NULL);
		}
		break;
	case 4:		/* Critical(4) */
		if (xdev->display) {
			battery_low = TRUE;
			ca_proplist_sets(pl, CA_PROP_EVENT_ID, "battery-caution");
			notify_notification_update(xdev->notify,
					"Warning",
					"The battery level is critically low.",
					"battery-caution");
			notify_notification_set_timeout(xdev->notify, 5);
			notify_notification_show(xdev->notify, NULL);
		}
		break;
	case 5:		/* Action(5) */
		if (xdev->display) {
			battery_low = TRUE;
			ca_proplist_sets(pl, CA_PROP_EVENT_ID, "battery-caution");
			notify_notification_update(xdev->notify,
					"Warning",
					"The battery level is critically low.",
					"battery-empty");
			notify_notification_set_timeout(xdev->notify, 5);
			notify_notification_show(xdev->notify, NULL);
		}
		break;
	}
	ca_context_play_full(ca, CaEventBatteryLevel, pl, NULL, NULL);
	ca_proplist_destroy(pl);
}

void
xde_device_level(XdeDevice *xdev, guint32 level, double percent)
{
	ca_context *ca = get_default_ca_context();
	ca_proplist *pl;

	notify_notification_close(xdev->notify, NULL);
	ca_context_cancel(ca, CaEventBatteryLevel);
	ca_proplist_create(&pl);

	switch (level) {
	default:
	case 0:		/* Unknown(0) */
		if (xdev->display && percent != 0.0) {
			battery_low = FALSE;
		}
		break;
	case 1:		/* None(1) */
		if (percent <= 5.0) {
			if (xdev->display && percent != 0.0) {
				battery_low = TRUE;
				ca_proplist_sets(pl, CA_PROP_EVENT_ID, "battery-caution");
				notify_notification_update(xdev->notify,
						"Warning",
						"The battery level is critically low.",
						"battery-caution");
				notify_notification_set_timeout(xdev->notify, 5);
			}
		} else if (percent <= 15.0) {
			if (xdev->display && percent != 0.0) {
				battery_low = TRUE;
				ca_proplist_sets(pl, CA_PROP_EVENT_ID, "battery-low");
				notify_notification_update(xdev->notify,
						"Warning",
						"The battery level is low.",
						"battery-low");
				notify_notification_set_timeout(xdev->notify, 5);
				notify_notification_show(xdev->notify, NULL);
			}
		} else if (percent >= 90.0) {
			if (xdev->display && percent != 0.0) {
				battery_low = FALSE;
			}
		} else {
			if (xdev->display && percent != 0.0) {
				battery_low = FALSE;
				ca_proplist_sets(pl, CA_PROP_EVENT_ID, "battery-full");
				notify_notification_update(xdev->notify,
						"Notice",
						"The battery is full.",
						"battery-full");
				notify_notification_set_timeout(xdev->notify, 5);
				notify_notification_show(xdev->notify, NULL);
			}
		}
		break;
	case 3:		/* Low(3) */
		if (xdev->display && percent != 0.0) {
			battery_low = TRUE;
			ca_proplist_sets(pl, CA_PROP_EVENT_ID, "battery-low");
			notify_notification_update(xdev->notify,
					"Warning",
					"The battery level is low.",
					"battery-low");
			notify_notification_set_timeout(xdev->notify, 5);
			notify_notification_show(xdev->notify, NULL);
		}
		break;
	case 4:		/* Critical(4) */
		if (xdev->display && percent != 0.0) {
			battery_low = TRUE;
			ca_proplist_sets(pl, CA_PROP_EVENT_ID, "battery-caution");
			notify_notification_update(xdev->notify,
					"Warning",
					"The battery level is low.",
					"battery-low");
			notify_notification_set_timeout(xdev->notify, 5);
			notify_notification_show(xdev->notify, NULL);
		}
		break;
	case 6:		/* Normal(6) */
		if (xdev->display && percent != 0.0) {
			battery_low = FALSE;
			notify_notification_update(xdev->notify,
					"Notice",
					"The battery level is good.",
					"battery-good");
			notify_notification_set_timeout(xdev->notify, 5);
			notify_notification_show(xdev->notify, NULL);
		}
		break;
	case 7:		/* High(7) */
		if (xdev->display && percent != 0.0) {
			battery_low = FALSE;
			ca_proplist_sets(pl, CA_PROP_EVENT_ID, "battery-full");
			notify_notification_update(xdev->notify,
					"Notice",
					"The battery level is high.",
					"battery-full");
			notify_notification_set_timeout(xdev->notify, 5);
			notify_notification_show(xdev->notify, NULL);
		}
		break;
	case 8:		/* Full(8) */
		if (xdev->display && percent != 0.0) {
			battery_low = FALSE;
			ca_proplist_sets(pl, CA_PROP_EVENT_ID, "battery-full");
			notify_notification_update(xdev->notify,
					"Notice",
					"The battery is full.",
					"battery-full");
			notify_notification_set_timeout(xdev->notify, 5);
			notify_notification_show(xdev->notify, NULL);
		}
		break;
	}
	ca_context_play_full(ca, CaEventBatteryLevel, pl, NULL, NULL);
	ca_proplist_destroy(pl);
}

void
on_up_device_proxy_props_changed(GDBusProxy *proxy, GVariant *changed_properties,
				 GStrv invalidated_properties, gpointer user_data)
{
	XdeDevice *xdev = user_data;
	GVariantIter iter;
	GVariant *prop;

	DPRINTF(1, "received upower devices proxy props change signal ( %s )\n",
		g_variant_get_type_string(changed_properties));
	g_variant_iter_init(&iter, changed_properties);
	while ((prop = g_variant_iter_next_value(&iter))) {
		GVariantIter iter2;
		GVariant *key;
		GVariant *boxed;
		GVariant *val;
		const gchar *name;

		g_variant_iter_init(&iter2, prop);
		if (!(key = g_variant_iter_next_value(&iter2))) {
			EPRINTF("no key!\n");
			continue;
		}
		if (!(name = g_variant_get_string(key, NULL))) {
			EPRINTF("no name!\n");
			g_variant_unref(key);
			continue;
		}
		if (strcmp(name, "IconName") && strcmp(name, "WarningLevel") && strcmp(name, "Percent")
		    && strcmp(name, "BatteryLevel")) {
			DPRINTF(1, "not looking for %s\n", name);
			g_variant_unref(key);
			continue;
		}
		if (!(boxed = g_variant_iter_next_value(&iter2))) {
			EPRINTF("no boxed!\n");
			g_variant_unref(key);
			continue;
		}
		if (!(val = g_variant_get_variant(boxed))) {
			EPRINTF("no value!\n");
			g_variant_unref(boxed);
			g_variant_unref(key);
			continue;
		}
		if (!strcmp(name, "IconName")) {
			if (xdev->display)
				update_display_icon(xdev, g_variant_get_string(val, NULL));
		} else if (!strcmp(name, "WarningLevel")) {
			xde_device_warn(xdev, g_variant_get_uint32(val));
		} else if (!strcmp(name, "Percentage")) {
			GVariant *batt;
			guint32 level = 1;

			if ((batt = g_dbus_proxy_get_cached_property(proxy, "BatteryLevel"))) {
				level = g_variant_get_uint32(batt);
				g_variant_unref(batt);
			}
			xde_device_level(xdev, level, g_variant_get_double(val));
		} else if (!strcmp(name, "BatteryLevel")) {
			GVariant *perc;
			double percent = 0.0;

			if ((perc = g_dbus_proxy_get_cached_property(proxy, "Percentage"))) {
				percent = g_variant_get_double(perc);
				g_variant_unref(perc);
			}
			xde_device_level(xdev, g_variant_get_uint32(val), percent);
		}
		g_variant_unref(val);
		g_variant_unref(boxed);
		g_variant_unref(key);
		g_variant_unref(prop);
	}
}

gint
xde_device_compare(gconstpointer a, gconstpointer b)
{
	const XdeDevice *xda = a;
	const XdeDevice *xdb = b;

	return strcmp(xda->path, xdb->path);
}

gint
xde_path_compare(gconstpointer a, gconstpointer b)
{
	const XdeDevice *xda = a;
	const gchar *path = b;

	return strcmp(xda->path, path);
}

void
xde_remove_device(const gchar *dev)
{
	GList *found;

	DPRINTF(1, "removing device %s\n", dev);
	if ((found = g_list_find_custom(up_devices, dev, xde_path_compare))) {
		xde_device_destroy(found->data);
		up_devices = g_list_delete_link(up_devices, found);
	}
}

void
xde_create_device(const gchar *dev, gboolean display)
{
	GDBusProxy *proxy;
	GError *err = NULL;
	XdeDevice *xdev;

	DPRINTF(1, "creating device %s\n", dev);
	if ((dev && (proxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM, 0, NULL,
							   "org.freedesktop.UPower", dev,
							   "org.freedesktop.UPower.Device", NULL, &err))) || err) {
		GVariant *prop;

		if (!(xdev = calloc(1, sizeof(*xdev)))) {
			g_object_unref(G_OBJECT(proxy));
			return;
		}
		xdev->path = g_strdup(dev);
		xdev->proxy = proxy;
		xdev->display = display;
		xdev->notify = notify_notification_new("Warning", "The battery level is low.", "battery-low");
		xde_remove_device(dev);
		up_devices = g_list_append(up_devices, xdev);
		g_signal_connect(G_OBJECT(proxy), "g-properties-changed",
				 G_CALLBACK(on_up_device_proxy_props_changed), xdev);
		if ((prop = g_dbus_proxy_get_cached_property(proxy, "IconName"))) {
			if (xdev->display)
				update_display_icon(xdev, g_variant_get_string(prop, NULL));
			g_variant_unref(prop);
		}
		if ((prop = g_dbus_proxy_get_cached_property(proxy, "WarningLevel"))) {
			xde_device_warn(xdev, g_variant_get_uint32(prop));
			g_variant_unref(prop);
		}
		if ((prop = g_dbus_proxy_get_cached_property(proxy, "Percentage"))) {
			double percent = g_variant_get_double(prop);

			g_variant_unref(prop);
			if ((prop = g_dbus_proxy_get_cached_property(proxy, "BatteryLevel"))) {
				guint32 level = g_variant_get_uint32(prop);

				g_variant_unref(prop);
				xde_device_level(xdev, level, percent);
			}
		}
		if (options.debug > 1)
			xde_device_dump(proxy);
	} else {
		EPRINTF("could not create proxy for device %s: %s\n", dev, err ? err->message : NULL);
		g_clear_error(&err);
	}
}

void
on_up_manager_proxy_signal(GDBusProxy *proxy, gchar *sender_name, gchar *signal_name,
			   GVariant *parameters, gpointer user_data)
{
	gchar *device = NULL;

	DPRINTF(1, "received upower manager proxy signal %s( %s )\n", signal_name,
		g_variant_get_type_string(parameters));
	if (!strcmp(signal_name, "DeviceAdded")) {
		g_variant_get(parameters, "(o)", &device);
		if (device) {
			DPRINTF(1, "device \"%s\" added\n", device);
			xde_create_device(device, FALSE);
			g_free(device);
		} else {
			EPRINTF("could not get added device name\n");
		}
	} else if (!strcmp(signal_name, "DeviceRemoved")) {
		g_variant_get(parameters, "(o)", &device);
		if (device) {
			DPRINTF(1, "device \"%s\" removed\n", device);
			xde_remove_device(device);
			g_free(device);
		} else {
			EPRINTF("could not get removed device name\n");
		}
	}
}

void
on_up_manager_proxy_props_changed(GDBusProxy *proxy, GVariant *changed_properties,
		GStrv invalidated_properties, gpointer user_data)
{
	GVariantIter iter;
	GVariant *prop;

	DPRINTF(1, "received upower proxy properties changed signal ( %s )\n",
			g_variant_get_type_string(changed_properties));
	g_variant_iter_init(&iter, changed_properties);
	while ((prop = g_variant_iter_next_value(&iter))) {
		if (g_variant_is_container(prop)) {
			GVariantIter iter2;
			GVariant *key;
			GVariant *val;
			GVariant *boxed;
			const gchar *name;

			g_variant_iter_init(&iter2, prop);
			if (!(key = g_variant_iter_next_value(&iter2))) {
				EPRINTF("no key!\n");
				continue;
			}
			if (!(name = g_variant_get_string(key, NULL))) {
				EPRINTF("no name!\n");
				g_variant_unref(key);
				continue;
			}
			if (strcmp(name, "OnBattery") && strcmp(name, "LidIsClosed") && strcmp(name, "LidIsPresent")) {
				DPRINTF(1, "not looking for %s\n", name);
				g_variant_unref(key);
				continue;
			}
			if (!(val = g_variant_iter_next_value(&iter2))) {
				EPRINTF("no val!\n");
				g_variant_unref(key);
				continue;
			}
			if (!(boxed = g_variant_get_variant(val))) {
				EPRINTF("no value!\n");
				g_variant_unref(val);
				g_variant_unref(key);
				continue;
			}
			gboolean setting = g_variant_get_boolean(boxed);
			ca_context *ca = get_default_ca_context();
			ca_proplist *pl;

			ca_context_cancel(ca, CaEventPowerChanged);
			ca_proplist_create(&pl);
			if (!strcmp(name, "OnBattery")) {
				if (setting) {
					if (battery_low) {
						ca_proplist_sets(pl, CA_PROP_EVENT_ID, "power-unplug-battery-low");
					} else {
						ca_proplist_sets(pl, CA_PROP_EVENT_ID, "power-unplug");
					}
				} else {
					ca_proplist_sets(pl, CA_PROP_EVENT_ID, "power-plug");
				}
			} else if (!strcmp(name, "LidIsClosed")) {
				if (setting) {
					ca_proplist_sets(pl, CA_PROP_EVENT_ID, "lid-close");
				} else {
					ca_proplist_sets(pl, CA_PROP_EVENT_ID, "lid-open");
				}
			} else if (!strcmp(name, "LidIsPresent")) {
			}
			ca_context_play_full(ca, CaEventPowerChanged, pl, NULL, NULL);
			ca_proplist_destroy(pl);

			g_variant_unref(boxed);
			g_variant_unref(val);
			g_variant_unref(key);
		}
		g_variant_unref(prop);
	}
}

void
init_upower(void)
{
	GVariant *result;
	GError *err = NULL;
	gchar *device = NULL;

	DPRINTF(1, "creating UPower manager proxy\n");
	if (!(up_manager = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM, 0, NULL,
							 "org.freedesktop.UPower",
							 "/org/freedesktop/UPower",
							 "org.freedesktop.UPower", NULL, &err)) || err) {
		EPRINTF("could not create DBUS proxy up_manager: %s\n", err ? err->message : NULL);
		g_clear_error(&err);
		return;
	} else {
		if (options.debug > 1)
			xde_manager_dump(up_manager);
	}
	g_signal_connect(G_OBJECT(up_manager), "g-signal", G_CALLBACK(on_up_manager_proxy_signal), NULL);
	g_signal_connect(G_OBJECT(up_manager), "g-properties-changed",
			 G_CALLBACK(on_up_manager_proxy_props_changed), NULL);

	if ((result = g_dbus_proxy_call_sync(up_manager, "GetCriticalAction", NULL,
					G_DBUS_CALL_FLAGS_NONE, -1, NULL, &err))) {
		g_variant_get(result, "(s)", &up_critical);
		DPRINTF(1, "UPower manager critical action: %s\n", up_critical);
	} else {
		EPRINTF("could not get critical action: %s\n", err ? err->message : NULL);
		g_clear_error(&err);
	}
	if ((result = g_dbus_proxy_call_sync(up_manager, "GetDisplayDevice", NULL,
					     G_DBUS_CALL_FLAGS_NONE, -1, NULL, &err))) {
		g_variant_get(result, "(o)", &device);
		g_variant_unref(result);
		DPRINTF(1, "UPower manager display device: %s\n", device);
	} else {
		EPRINTF("could not get display device: %s\n", err ? err->message : NULL);
		g_clear_error(&err);
	}
	DPRINTF(1, "creating UPower manager display device proxy\n");
	xde_create_device("/org/freedesktop/UPower/devices/DisplayDevice", TRUE);
	DPRINTF(1, "enumerating UPower manager devices\n");
	if ((result = g_dbus_proxy_call_sync(up_manager, "EnumerateDevices", NULL,
					G_DBUS_CALL_FLAGS_NONE, -1, NULL, &err))) {
		GVariantIter iter;
		GVariant *array;

		g_variant_iter_init(&iter, result);
		while ((array = g_variant_iter_next_value(&iter))) {
			GVariantIter iter2;
			GVariant *object;

			g_variant_iter_init(&iter2, array);
			while ((object = g_variant_iter_next_value(&iter2))) {
				xde_create_device(g_variant_get_string(object, NULL), FALSE);
				g_variant_unref(object);
			}
			g_variant_unref(array);
		}
		g_variant_unref(result);
	} else {
		EPRINTF("could not enumerate devices: %s\n", err ? err->message : NULL);
		g_clear_error(&err);
	}
}

static Window
get_selection(Bool replace, Window selwin)
{
	char selection[64] = { 0, };
	GdkDisplay *disp;
	Display *dpy;
	int s, nscr;
	Atom atom;
	Window owner, gotone = None;

	disp = gdk_display_get_default();
	nscr = gdk_display_get_n_screens(disp);

	dpy = GDK_DISPLAY_XDISPLAY(disp);

	for (s = 0; s < nscr; s++) {
		snprintf(selection, sizeof(selection), XA_SELECTION_NAME, s);
		atom = XInternAtom(dpy, selection, False);
		if (!(owner = XGetSelectionOwner(dpy, atom)))
			DPRINTF(1, "No owner for %s\n", selection);
		if ((owner && replace) || (!owner && selwin)) {
			DPRINTF(1, "Setting owner of %s to 0x%08lx from 0x%08lx\n", selection, selwin,
				owner);
			XSetSelectionOwner(dpy, atom, selwin, CurrentTime);
			XSync(dpy, False);
			/* XXX: should do XIfEvent for owner window destruction */
		}
		if (!gotone && owner)
			gotone = owner;
	}
	if (replace) {
		if (gotone) {
			if (selwin)
				DPRINTF(1, "%s: replacing running instance\n", NAME);
			else
				DPRINTF(1, "%s: quitting running instance\n", NAME);
		} else {
			if (selwin)
				DPRINTF(1, "%s: no running instance to replace\n", NAME);
			else
				DPRINTF(1, "%s: no running instance to quit\n", NAME);
		}
		if (selwin) {
			XEvent ev;

			for (s = 0; s < nscr; s++) {
				snprintf(selection, sizeof(selection), XA_SELECTION_NAME, s);

				ev.xclient.type = ClientMessage;
				ev.xclient.serial = 0;
				ev.xclient.send_event = False;
				ev.xclient.display = dpy;
				ev.xclient.window = RootWindow(dpy, s);
				ev.xclient.message_type = _XA_MANAGER;
				ev.xclient.format = 32;
				ev.xclient.data.l[0] = CurrentTime;
				ev.xclient.data.l[1] = XInternAtom(dpy, selection, False);
				ev.xclient.data.l[2] = selwin;
				ev.xclient.data.l[3] = 0;
				ev.xclient.data.l[4] = 0;

				XSendEvent(dpy, RootWindow(dpy, s), False, StructureNotifyMask,
					   &ev);
				XFlush(dpy);
			}
		}
	} else if (gotone)
		DPRINTF(1, "%s: not replacing running instance\n", NAME);
	return (gotone);
}

static void
window_manager_changed(WnckScreen *wnck, gpointer user)
{
	XdeScreen *xscr = user;
	const char *name;

	/* I suppose that what we should do here is set a timer and wait before doing
	   anything; however, I think that libwnck++ already does this (waits before even
	   giving us the signal). */

	if (!xscr) {
		EPRINTF("xscr is NULL\n");
		exit(EXIT_FAILURE);
	}
	wnck_screen_force_update(wnck);
	free(xscr->wmname);
	xscr->wmname = NULL;
	xscr->goodwm = False;
	/* FIXME: need to check and free tree, xsessions and menu in old context before
	   assigning a new one. */
	if ((name = wnck_screen_get_window_manager_name(wnck))) {
		xscr->wmname = strdup(name);
		*strchrnul(xscr->wmname, ' ') = '\0';
		/* Some versions of wmx have an error in that they only set the
		   _NET_WM_NAME to the first letter of wmx. */
		if (!strcmp(xscr->wmname, "w")) {
			free(xscr->wmname);
			xscr->wmname = strdup("wmx");
		}
		/* Ahhhh, the strange naming of μwm...  Unfortunately there are several
		   ways to make a μ in utf-8!!! */
		if (!strcmp(xscr->wmname, "\xce\xbcwm") || !strcmp(xscr->wmname, "\xc2\xb5wm")) {
			free(xscr->wmname);
			xscr->wmname = strdup("uwm");
		}
	}
	DPRINTF(1, "window manager is '%s'\n", xscr->wmname);
	DPRINTF(1, "window manager is %s\n", xscr->goodwm ? "usable" : "unusable");
}

static void
init_wnck(XdeScreen *xscr)
{
	WnckScreen *wnck = xscr->wnck = wnck_screen_get(xscr->index);

	g_signal_connect(G_OBJECT(wnck), "window_manager_changed",
			 G_CALLBACK(window_manager_changed), xscr);

	window_manager_changed(wnck, xscr);
}

static void
update_theme(XdeScreen *xscr, Atom prop)
{
	Display *dpy = GDK_DISPLAY_XDISPLAY(xscr->disp);
	Window root = RootWindow(dpy, xscr->index);
	XTextProperty xtp = { NULL, };
	Bool changed = False;
	GtkSettings *set;

	gtk_rc_reparse_all();
	if (!prop || prop == _XA_GTK_READ_RCFILES)
		prop = _XA_XDE_THEME_NAME;
	if (XGetTextProperty(dpy, root, &xtp, prop)) {
		char **list = NULL;
		int strings = 0;

		if (Xutf8TextPropertyToTextList(dpy, &xtp, &list, &strings) == Success) {
			if (strings >= 1) {
				static const char *prefix = "gtk-theme-name=\"";
				static const char *suffix = "\"";
				char *rc_string;
				int len;

				len = strlen(prefix) + strlen(list[0]) + strlen(suffix);
				rc_string = calloc(len + 1, sizeof(*rc_string));
				strncpy(rc_string, prefix, len);
				strncat(rc_string, list[0], len);
				strncat(rc_string, suffix, len);
				gtk_rc_parse_string(rc_string);
				free(rc_string);
				if (!xscr->theme || strcmp(xscr->theme, list[0])) {
					free(xscr->theme);
					xscr->theme = strdup(list[0]);
					changed = True;
				}
			}
			if (list)
				XFreeStringList(list);
		} else
			DPRINTF(1, "could not get text list for property\n");
		if (xtp.value)
			XFree(xtp.value);
	} else
		DPRINTF(1, "could not get %s for root 0x%lx\n", XGetAtomName(dpy, prop), root);
	if ((set = gtk_settings_get_for_screen(xscr->scrn))) {
		GValue theme_v = G_VALUE_INIT;
		const char *theme;

		g_value_init(&theme_v, G_TYPE_STRING);
		g_object_get_property(G_OBJECT(set), "gtk-theme-name", &theme_v);
		theme = g_value_get_string(&theme_v);
		if (theme && (!xscr->theme || strcmp(xscr->theme, theme))) {
			free(xscr->theme);
			xscr->theme = strdup(theme);
			changed = True;
		}
		g_value_unset(&theme_v);
	}
	if (changed) {
		DPRINTF(1, "New theme is %s\n", xscr->theme);
		/* FIXME: do somthing more about it. */
	}
}

static void
update_icon_theme(XdeScreen *xscr, Atom prop)
{
	Display *dpy = GDK_DISPLAY_XDISPLAY(xscr->disp);
	Window root = RootWindow(dpy, xscr->index);
	XTextProperty xtp = { NULL, };
	Bool changed = False;
	GtkSettings *set;

	gtk_rc_reparse_all();
	if (!prop || prop == _XA_GTK_READ_RCFILES)
		prop = _XA_XDE_ICON_THEME_NAME;
	if (XGetTextProperty(dpy, root, &xtp, prop)) {
		char **list = NULL;
		int strings = 0;

		if (Xutf8TextPropertyToTextList(dpy, &xtp, &list, &strings) == Success) {
			if (strings >= 1) {
				static const char *prefix = "gtk-icon-theme-name=\"";
				static const char *suffix = "\"";
				char *rc_string;
				int len;

				len = strlen(prefix) + strlen(list[0]) + strlen(suffix);
				rc_string = calloc(len + 1, sizeof(*rc_string));
				strncpy(rc_string, prefix, len);
				strncat(rc_string, list[0], len);
				strncat(rc_string, suffix, len);
				gtk_rc_parse_string(rc_string);
				free(rc_string);
				if (!xscr->itheme || strcmp(xscr->itheme, list[0])) {
					free(xscr->itheme);
					xscr->itheme = strdup(list[0]);
					changed = True;
				}
			}
			if (list)
				XFreeStringList(list);
		} else
			DPRINTF(1, "could not get text list for property\n");
		if (xtp.value)
			XFree(xtp.value);
	} else
		DPRINTF(1, "could not get %s for root 0x%lx\n", XGetAtomName(dpy, prop), root);
	if ((set = gtk_settings_get_for_screen(xscr->scrn))) {
		GValue theme_v = G_VALUE_INIT;
		const char *itheme;

		g_value_init(&theme_v, G_TYPE_STRING);
		g_object_get_property(G_OBJECT(set), "gtk-icon-theme-name", &theme_v);
		itheme = g_value_get_string(&theme_v);
		if (itheme && (!xscr->itheme || strcmp(xscr->itheme, itheme))) {
			free(xscr->itheme);
			xscr->itheme = strdup(itheme);
			changed = True;
		}
		g_value_unset(&theme_v);
	}
	if (changed) {
		DPRINTF(1, "New icon theme is %s\n", xscr->itheme);
		/* FIXME: do something more about it. */
	}
}

static GdkFilterReturn selwin_handler(GdkXEvent *xevent, GdkEvent *event, gpointer data);
static GdkFilterReturn root_handler(GdkXEvent *xevent, GdkEvent *event, gpointer data);
static GdkFilterReturn owner_handler(GdkXEvent *xevent, GdkEvent *event, gpointer data);

static void
setup_x11(Bool replace)
{
	GdkDisplay *disp;
	Display *dpy;
	GdkScreen *scrn;
	GdkWindow *root, *sel, *own;
	char selection[64] = { 0, };
	Window selwin, owner;
	XdeScreen *xscr;
	int s, nscr;

	DPRINTF(1, "getting default GDK display\n");
	disp = gdk_display_get_default();
	DPRINTF(1, "getting default display\n");
	dpy = GDK_DISPLAY_XDISPLAY(disp);
	DPRINTF(1, "getting default GDK screen\n");
	scrn = gdk_display_get_default_screen(disp);
	DPRINTF(1, "getting default GDK root window\n");
	root = gdk_screen_get_root_window(scrn);

	DPRINTF(1, "creating select window\n");
	selwin = XCreateSimpleWindow(dpy, GDK_WINDOW_XID(root), 0, 0, 1, 1, 0, 0, 0);

	DPRINTF(1, "checking for selection\n");
	if ((owner = get_selection(replace, selwin))) {
		if (!replace) {
			XDestroyWindow(dpy, selwin);
			EPRINTF("%s: instance 0x%08lx is already running\n", NAME, owner);
			exit(EXIT_FAILURE);
		}
	}
	DPRINTF(1, "selecting inputs on 0x%08lx\n", selwin);
	XSelectInput(dpy, selwin,
		     StructureNotifyMask | SubstructureNotifyMask | PropertyChangeMask);

	DPRINTF(1, "getting number of screens\n");
	nscr = gdk_display_get_n_screens(disp);
	DPRINTF(1, "allocating %d screen structures\n", nscr);
	screens = calloc(nscr, sizeof(*screens));

	DPRINTF(1, "getting GDK window for 0x%08lx\n", selwin);
	sel = gdk_x11_window_foreign_new_for_display(disp, selwin);
	DPRINTF(1, "adding a filter for the select window\n");
	gdk_window_add_filter(sel, selwin_handler, screens);

	DPRINTF(1, "initializing %d screens\n", nscr);
	for (s = 0, xscr = screens; s < nscr; s++, xscr++) {
		snprintf(selection, sizeof(selection), XA_SELECTION_NAME, s);
		xscr->index = s;
		xscr->atom = XInternAtom(dpy, selection, False);
		xscr->disp = disp;
		xscr->scrn = gdk_display_get_screen(disp, s);
		xscr->root = gdk_screen_get_root_window(xscr->scrn);
		xscr->selwin = selwin;
		if ((xscr->owner = XGetSelectionOwner(dpy, xscr->atom)) && xscr->owner != selwin) {
			XSelectInput(dpy, xscr->owner,
				     StructureNotifyMask | SubstructureNotifyMask |
				     PropertyChangeMask);
			own = gdk_x11_window_foreign_new_for_display(disp, xscr->owner);
			gdk_window_add_filter(own, owner_handler, xscr);
		}
		gdk_window_add_filter(xscr->root, root_handler, xscr);
#ifdef DESKTOP_NOTIFICATIONS
		notify_init(RESNAME);
#endif
		init_wnck(xscr);
		update_theme(xscr, None);
		update_icon_theme(xscr, None);
		if (options.tray)
			init_statusicon(xscr);
		if (options.dock)
			init_dockapp(xscr);
		init_upower();
	}
}

static GdkFilterReturn
event_handler_ClientMessage(Display *dpy, XEvent *xev)
{
	XdeScreen *xscr = NULL;
	int s, nscr = ScreenCount(dpy);

	for (s = 0; s < nscr; s++)
		if (xev->xclient.window == RootWindow(dpy, s))
			xscr = screens + s;
	if (options.debug) {
		fprintf(stderr, "==> ClientMessage: %p\n", xscr);
		fprintf(stderr, "    --> window = 0x%08lx\n", xev->xclient.window);
		fprintf(stderr, "    --> message_type = %s\n",
			XGetAtomName(dpy, xev->xclient.message_type));
		fprintf(stderr, "    --> format = %d\n", xev->xclient.format);
		switch (xev->xclient.format) {
			int i;

		case 8:
			fprintf(stderr, "    --> data =");
			for (i = 0; i < 20; i++)
				fprintf(stderr, " %02x", (int) xev->xclient.data.b[i]);
			fprintf(stderr, "\n");
			break;
		case 16:
			fprintf(stderr, "    --> data =");
			for (i = 0; i < 10; i++)
				fprintf(stderr, " %04x", (int) xev->xclient.data.s[i]);
			fprintf(stderr, "\n");
			break;
		case 32:
			fprintf(stderr, "    --> data =");
			for (i = 0; i < 20; i++)
				fprintf(stderr, " %08lx", xev->xclient.data.l[i]);
			fprintf(stderr, "\n");
			break;
		}
		fprintf(stderr, "<== ClientMessage: %p\n", xscr);
	}
	if (xscr && xev->xclient.message_type == _XA_GTK_READ_RCFILES) {
		update_theme(xscr, xev->xclient.message_type);
		update_icon_theme(xscr, xev->xclient.message_type);
		return GDK_FILTER_REMOVE;
	} else if (xscr && xev->xclient.message_type == _XA_XDE_UP_APPLET_REFRESH) {
		// set_flags(xev->xclient.data.l[2]);
		// set_word1(xev->xclient.data.l[3]);
		// set_word2(xev->xclient.data.l[4]);
		applet_refresh(xscr);
		return GDK_FILTER_REMOVE;
	} else if (xscr && xev->xclient.message_type == _XA_XDE_UP_APPLET_RESTART) {
		// set_flags(xev->xclient.data.l[2]);
		// set_word1(xev->xclient.data.l[3]);
		// set_word2(xev->xclient.data.l[4]);
		applet_restart();
		return GDK_FILTER_REMOVE;
	} else if (xscr && xev->xclient.message_type == _XA_XDE_UP_APPLET_POPMENU) {
		// set_flags(xev->xclient.data.l[2]);
		// set_word1(xev->xclient.data.l[3]);
		// set_word2(xev->xclient.data.l[4]);
		// menu_popmenu(xscr);
		return GDK_FILTER_REMOVE;
	}
	return GDK_FILTER_CONTINUE;
}

static GdkFilterReturn
event_handler_PropertyNotify(Display *dpy, XEvent *xev, XdeScreen *xscr)
{
	Atom atom;

	if (options.debug > 2) {
		fprintf(stderr, "==> PropertyNotify:\n");
		fprintf(stderr, "    --> window = 0x%08lx\n", xev->xproperty.window);
		fprintf(stderr, "    --> atom = %s\n", XGetAtomName(dpy, xev->xproperty.atom));
		fprintf(stderr, "    --> time = %ld\n", xev->xproperty.time);
		fprintf(stderr, "    --> state = %s\n",
			(xev->xproperty.state == PropertyNewValue) ? "NewValue" : "Delete");
		fprintf(stderr, "<== PropertyNotify:\n");
	}
	atom = xev->xproperty.atom;

	if (xev->xproperty.state == PropertyNewValue) {
		if (atom == _XA_XDE_THEME_NAME || atom == _XA_XDE_WM_THEME) {
			update_theme(xscr, xev->xproperty.atom);
			return GDK_FILTER_REMOVE;
		} else if (atom == _XA_XDE_ICON_THEME_NAME || atom == _XA_XDE_WM_ICONTHEME) {
			update_icon_theme(xscr, xev->xproperty.atom);
			return GDK_FILTER_REMOVE;
		}
	}
	return GDK_FILTER_CONTINUE;
}

static GdkFilterReturn
root_handler(GdkXEvent *xevent, GdkEvent *event, gpointer data)
{
	XEvent *xev = (typeof(xev)) xevent;
	XdeScreen *xscr = (typeof(xscr)) data;
	Display *dpy = GDK_DISPLAY_XDISPLAY(xscr->disp);

	PTRACE(1);
	if (!xscr) {
		EPRINTF("xscr is NULL\n");
		exit(EXIT_FAILURE);
	}
	switch (xev->type) {
	case PropertyNotify:
		return event_handler_PropertyNotify(dpy, xev, xscr);
	case ClientMessage:
		return event_handler_ClientMessage(dpy, xev);
	}
	return GDK_FILTER_CONTINUE;
}

static GdkFilterReturn
event_handler_SelectionClear(Display *dpy, XEvent *xev, XdeScreen *xscr)
{
	PTRACE(1);
	if (options.debug > 1) {
		fprintf(stderr, "==> SelectionClear: %p\n", xscr);
		fprintf(stderr, "    --> send_event = %s\n",
			xev->xselectionclear.send_event ? "true" : "false");
		fprintf(stderr, "    --> window = 0x%08lx\n", xev->xselectionclear.window);
		fprintf(stderr, "    --> selection = %s\n",
			XGetAtomName(dpy, xev->xselectionclear.selection));
		fprintf(stderr, "    --> time = %lu\n", xev->xselectionclear.time);
		fprintf(stderr, "<== SelectionClear: %p\n", xscr);
	}
	if (xscr && xev->xselectionclear.window == xscr->selwin) {
		XDestroyWindow(dpy, xscr->selwin);
		DPRINTF(1, "selection cleared, exiting\n");
		if (smcConn) {
			/* Care must be taken where if we are running under a session
			   manager. We set the restart hint to SmRestartImmediately
			   which means that the session manager will re-execute us if we
			   exit.  We should really request a local shutdown. */
			SmcRequestSaveYourself(smcConn, SmSaveLocal, True, SmInteractStyleNone,
					       False, False);
			return GDK_FILTER_CONTINUE;
		}
		exit(EXIT_SUCCESS);
	}
	return GDK_FILTER_CONTINUE;
}

static GdkFilterReturn
event_handler_SelectionRequest(Display *dpy, XEvent *xev, XdeScreen *xscr)
{
	PTRACE(1);
	if (options.debug > 1) {
		fprintf(stderr, "==> SelectionRequest: %p\n", xscr);
		fprintf(stderr, "    --> send_event = %s\n",
			xev->xselectionrequest.send_event ? "true" : "false");
		fprintf(stderr, "    --> owner = 0x%08lx\n", xev->xselectionrequest.owner);
		fprintf(stderr, "    --> requestor = 0x%08lx\n", xev->xselectionrequest.requestor);
		fprintf(stderr, "    --> selection = %s\n",
			XGetAtomName(dpy, xev->xselectionrequest.selection));
		fprintf(stderr, "    --> target = %s\n",
			XGetAtomName(dpy, xev->xselectionrequest.target));
		fprintf(stderr, "    --> property = %s\n",
			XGetAtomName(dpy, xev->xselectionrequest.property));
		fprintf(stderr, "    --> time = %lu\n", xev->xselectionrequest.time);
		fprintf(stderr, "<== SelectionRequest: %p\n", xscr);
	}
	if (xscr && xev->xselectionrequest.owner == xscr->owner) {
	}
	return GDK_FILTER_CONTINUE;
}

static GdkFilterReturn
event_handler_DestroyNotify(Display *dpy, XEvent *xev, XdeScreen *xscr)
{
	PTRACE(1);
	if (options.debug > 1) {
		fprintf(stderr, "==> DestroyNotify: %p\n", xscr);
		fprintf(stderr, "    --> send_event = %s\n",
				xev->xdestroywindow.send_event ? "true" : "false");
		fprintf(stderr, "    --> event = 0x%08lx\n", xev->xdestroywindow.event);
		fprintf(stderr, "    --> window = 0x%08lx\n", xev->xdestroywindow.window);
		fprintf(stderr, "<== DestroyNotify: %p\n", xscr);
	}
	if (xscr && xev->xdestroywindow.window == xscr->owner) {
	}
	return GDK_FILTER_CONTINUE;
}

static GdkFilterReturn
selwin_handler(GdkXEvent *xevent, GdkEvent *event, gpointer data)
{
	XEvent *xev = (typeof(xev)) xevent;
	XdeScreen *xscr = data;
	Display *dpy = GDK_DISPLAY_XDISPLAY(xscr->disp);

	PTRACE(1);
	if (!xscr) {
		EPRINTF("xscr is NULL\n");
		exit(EXIT_FAILURE);
	}
	switch (xev->type) {
	case SelectionClear:
		return event_handler_SelectionClear(dpy, xev, xscr);
	case SelectionRequest:
		return event_handler_SelectionRequest(dpy, xev, xscr);
	case ClientMessage:
		return event_handler_ClientMessage(dpy, xev);
	}
	EPRINTF("wrong message type for handler %d\n", xev->type);
	return GDK_FILTER_CONTINUE;
}

static GdkFilterReturn
owner_handler(GdkXEvent *xevent, GdkEvent *event, gpointer data)
{
	XEvent *xev = (typeof(xev)) xevent;
	XdeScreen *xscr = data;
	Display *dpy = GDK_DISPLAY_XDISPLAY(xscr->disp);
	switch (xev->type) {
	case DestroyNotify:
		return event_handler_DestroyNotify(dpy, xev, xscr);
	case ClientMessage:
		return event_handler_ClientMessage(dpy, xev);
	}
	EPRINTF("wrong message type for handler %d\n", xev->type);
	return GDK_FILTER_CONTINUE;
}

static GdkFilterReturn
client_handler(GdkXEvent *xevent, GdkEvent *event, gpointer data)
{
	XEvent *xev = (typeof(xev)) xevent;
	Display *dpy = (typeof(dpy)) data;

	PTRACE(1);
	switch (xev->type) {
	case ClientMessage:
		return event_handler_ClientMessage(dpy, xev);
	}
	EPRINTF("wrong message type for handler %d\n", xev->type);
	return GDK_FILTER_CONTINUE;
}

static void
clientSetProperties(SmcConn smcConn, SmPointer data)
{
	char userID[20];
	int i, j, argc = saveArgc;
	char **argv = saveArgv;
	char *cwd = NULL;
	char hint;
	struct passwd *pw;
	SmPropValue *penv = NULL, *prst = NULL, *pcln = NULL;
	SmPropValue propval[11];
	SmProp prop[11];

	SmProp *props[11] = {
		&prop[0], &prop[1], &prop[2], &prop[3], &prop[4],
		&prop[5], &prop[6], &prop[7], &prop[8], &prop[9],
		&prop[10]
	};

	j = 0;

	/* CloneCommand: This is like the RestartCommand except it restarts a copy of the
	   application.  The only difference is that the application doesn't supply its
	   client id at register time.  On POSIX systems the type should be a
	   LISTofARRAY8. */
	prop[j].name = SmCloneCommand;
	prop[j].type = SmLISTofARRAY8;
	prop[j].vals = pcln = calloc(argc, sizeof(*pcln));
	prop[j].num_vals = 0;
	props[j] = &prop[j];
	for (i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "-clientId") || !strcmp(argv[i], "-restore"))
			i++;
		else {
			prop[j].vals[prop[j].num_vals].value = (SmPointer) argv[i];
			prop[j].vals[prop[j].num_vals++].length = strlen(argv[i]);
		}
	}
	j++;

#if 1
	/* CurrentDirectory: On POSIX-based systems, specifies the value of the current
	   directory that needs to be set up prior to starting the program and should be
	   of type ARRAY8. */
	prop[j].name = SmCurrentDirectory;
	prop[j].type = SmARRAY8;
	prop[j].vals = &propval[j];
	prop[j].num_vals = 1;
	props[j] = &prop[j];
	propval[j].value = NULL;
	propval[j].length = 0;
	cwd = calloc(PATH_MAX + 1, sizeof(propval[j].value[0]));
	if (getcwd(cwd, PATH_MAX)) {
		propval[j].value = cwd;
		propval[j].length = strlen(propval[j].value);
		j++;
	} else {
		free(cwd);
		cwd = NULL;
	}
#endif

#if 0
	/* DiscardCommand: The discard command contains a command that when delivered to
	   the host that the client is running on (determined from the connection), will
	   cause it to discard any information about the current state.  If this command
	   is not specified, the SM will assume that all of the client's state is encoded
	   in the RestartCommand [and properties].  On POSIX systems the type should be
	   LISTofARRAY8. */
	prop[j].name = SmDiscardCommand;
	prop[j].type = SmLISTofARRAY8;
	prop[j].vals = &propval[j];
	prop[j].num_vals = 1;
	props[j] = &prop[j];
	propval[j].value = "/bin/true";
	propval[j].length = strlen("/bin/true");
	j++;
#endif

#if 1
	char **env;

	/* Environment: On POSIX based systems, this will be of type LISTofARRAY8 where
	   the ARRAY8s alternate between environment variable name and environment
	   variable value. */
	/* XXX: we might want to filter a few out */
	for (i = 0, env = environ; *env; i += 2, env++) ;
	prop[j].name = SmEnvironment;
	prop[j].type = SmLISTofARRAY8;
	prop[j].vals = penv = calloc(i, sizeof(*penv));
	prop[j].num_vals = i;
	props[j] = &prop[j];
	for (i = 0, env = environ; *env; i += 2, env++) {
		char *equal;
		int len;

		equal = strchrnul(*env, '=');
		len = (int) (*env - equal);
		if (*equal)
			equal++;
		prop[j].vals[i].value = *env;
		prop[j].vals[i].length = len;
		prop[j].vals[i + 1].value = equal;
		prop[j].vals[i + 1].length = strlen(equal);
	}
	j++;
#endif

#if 1
	char procID[20];

	/* ProcessID: This specifies an OS-specific identifier for the process. On POSIX
	   systems this should be of type ARRAY8 and contain the return of getpid()
	   turned into a Latin-1 (decimal) string. */
	prop[j].name = SmProcessID;
	prop[j].type = SmARRAY8;
	prop[j].vals = &propval[j];
	prop[j].num_vals = 1;
	props[j] = &prop[j];
	snprintf(procID, sizeof(procID), "%ld", (long) getpid());
	propval[j].value = procID;
	propval[j].length = strlen(procID);
	j++;
#endif

	/* Program: The name of the program that is running.  On POSIX systems, this
	   should eb the first parameter passed to execve(3) and should be of type
	   ARRAY8. */
	prop[j].name = SmProgram;
	prop[j].type = SmARRAY8;
	prop[j].vals = &propval[j];
	prop[j].num_vals = 1;
	props[j] = &prop[j];
	propval[j].value = argv[0];
	propval[j].length = strlen(argv[0]);
	j++;

	/* RestartCommand: The restart command contains a command that when delivered to
	   the host that the client is running on (determined from the connection), will
	   cause the client to restart in its current state.  On POSIX-based systems this
	   if of type LISTofARRAY8 and each of the elements in the array represents an
	   element in the argv[] array.  This restart command should ensure that the
	   client restarts with the specified client-ID.  */
	prop[j].name = SmRestartCommand;
	prop[j].type = SmLISTofARRAY8;
	prop[j].vals = prst = calloc(argc + 4, sizeof(*prst));
	prop[j].num_vals = 0;
	props[j] = &prop[j];
	for (i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "-clientId") || !strcmp(argv[i], "-restore"))
			i++;
		else {
			prop[j].vals[prop[j].num_vals].value = (SmPointer) argv[i];
			prop[j].vals[prop[j].num_vals++].length = strlen(argv[i]);
		}
	}
	prop[j].vals[prop[j].num_vals].value = (SmPointer) "-clientId";
	prop[j].vals[prop[j].num_vals++].length = 9;
	prop[j].vals[prop[j].num_vals].value = (SmPointer) options.clientId;
	prop[j].vals[prop[j].num_vals++].length = strlen(options.clientId);

	prop[j].vals[prop[j].num_vals].value = (SmPointer) "-restore";
	prop[j].vals[prop[j].num_vals++].length = 9;
	prop[j].vals[prop[j].num_vals].value = (SmPointer) options.saveFile;
	prop[j].vals[prop[j].num_vals++].length = strlen(options.saveFile);
	j++;

	/* ResignCommand: A client that sets the RestartStyleHint to RestartAnyway uses
	   this property to specify a command that undoes the effect of the client and
	   removes any saved state. */
	prop[j].name = SmResignCommand;
	prop[j].type = SmLISTofARRAY8;
	prop[j].vals = calloc(2, sizeof(*prop[j].vals));
	prop[j].num_vals = 2;
	props[j] = &prop[j];
	prop[j].vals[0].value = "/usr/bin/xde-pager";
	prop[j].vals[0].length = strlen("/usr/bin/xde-pager");
	prop[j].vals[1].value = "-quit";
	prop[j].vals[1].length = strlen("-quit");
	j++;

	/* RestartStyleHint: If the RestartStyleHint property is present, it will contain
	   the style of restarting the client prefers.  If this flag is not specified,
	   RestartIfRunning is assumed.  The possible values are as follows:
	   RestartIfRunning(0), RestartAnyway(1), RestartImmediately(2), RestartNever(3).
	   The RestartIfRunning(0) style is used in the usual case.  The client should be
	   restarted in the next session if it is connected to the session manager at the
	   end of the current session. The RestartAnyway(1) style is used to tell the SM
	   that the application should be restarted in the next session even if it exits
	   before the current session is terminated. It should be noted that this is only
	   a hint and the SM will follow the policies specified by its users in
	   determining what applications to restart.  A client that uses RestartAnyway(1)
	   should also set the ResignCommand and ShutdownCommand properties to the
	   commands that undo the state of the client after it exits.  The
	   RestartImmediately(2) style is like RestartAnyway(1) but in addition, the
	   client is meant to run continuously.  If the client exits, the SM should try to
	   restart it in the current session.  The RestartNever(3) style specifies that
	   the client does not wish to be restarted in the next session. */
	prop[j].name = SmRestartStyleHint;
	prop[j].type = SmARRAY8;
	prop[j].vals = &propval[0];
	prop[j].num_vals = 1;
	props[j] = &prop[j];
	hint = SmRestartImmediately;	/* <--- */
	propval[j].value = &hint;
	propval[j].length = 1;
	j++;

	/* ShutdownCommand: This command is executed at shutdown time to clean up after a
	   client that is no longer running but retained its state by setting
	   RestartStyleHint to RestartAnyway(1).  The command must not remove any saved
	   state as the client is still part of the session. */
	prop[j].name = SmShutdownCommand;
	prop[j].type = SmLISTofARRAY8;
	prop[j].vals = calloc(2, sizeof(*prop[j].vals));
	prop[j].num_vals = 2;
	props[j] = &prop[j];
	prop[j].vals[0].value = "/usr/bin/xde-pager";
	prop[j].vals[0].length = strlen("/usr/bin/xde-pager");
	prop[j].vals[1].value = "-quit";
	prop[j].vals[1].length = strlen("-quit");
	j++;

	/* UserID: Specifies the user's ID.  On POSIX-based systems this will contain the
	   user's name (the pw_name field of struct passwd).  */
	errno = 0;
	prop[j].name = SmUserID;
	prop[j].type = SmARRAY8;
	prop[j].vals = &propval[j];
	prop[j].num_vals = 1;
	props[j] = &prop[j];
	if ((pw = getpwuid(getuid())))
		strncpy(userID, pw->pw_name, sizeof(userID) - 1);
	else {
		EPRINTF("%s: %s\n", "getpwuid()", strerror(errno));
		snprintf(userID, sizeof(userID), "%ld", (long) getuid());
	}
	propval[j].value = userID;
	propval[j].length = strlen(userID);
	j++;

	SmcSetProperties(smcConn, j, props);

	free(cwd);
	free(pcln);
	free(prst);
	free(penv);
}

static Bool saving_yourself;
static Bool shutting_down;

static void
clientSaveYourselfPhase2CB(SmcConn smcConn, SmPointer data)
{
	clientSetProperties(smcConn, data);
	SmcSaveYourselfDone(smcConn, True);
}

/** @brief save yourself
  *
  * The session manager sends a "Save Yourself" message to a client either to
  * check-point it or just before termination so that it can save its state.
  * The client responds with zero or more calls to SmcSetProperties to update
  * the properties indicating how to restart the client.  When all the
  * properties have been set, the client calls SmcSaveYourselfDone.
  *
  * If interact_type is SmcInteractStyleNone, the client must not interact with
  * the user while saving state.  If interact_style is SmInteractStyleErrors,
  * the client may interact with the user only if an error condition arises.  If
  * interact_style is  SmInteractStyleAny then the client may interact with the
  * user for any purpose.  Because only one client can interact with the user at
  * a time, the client must call SmcInteractRequest and wait for an "Interact"
  * message from the session maanger.  When the client is done interacting with
  * the user, it calls SmcInteractDone.  The client may only call
  * SmcInteractRequest() after it receives a "Save Yourself" message and before
  * it calls SmcSaveYourSelfDone().
  */
static void
clientSaveYourselfCB(SmcConn smcConn, SmPointer data, int saveType, Bool shutdown,
		     int interactStyle, Bool fast)
{
	if (!(shutting_down = shutdown)) {
		if (!SmcRequestSaveYourselfPhase2(smcConn, clientSaveYourselfPhase2CB, data))
			SmcSaveYourselfDone(smcConn, False);
		return;
	}
	clientSetProperties(smcConn, data);
	SmcSaveYourselfDone(smcConn, True);
}

/** @brief die
  *
  * The session manager sends a "Die" message to a client when it wants it to
  * die.  The client should respond by calling SmcCloseConnection.  A session
  * manager that behaves properly will send a "Save Yourself" message before the
  * "Die" message.
  */
static void
clientDieCB(SmcConn smcConn, SmPointer data)
{
	SmcCloseConnection(smcConn, 0, NULL);
	shutting_down = False;
	gtk_main_quit();
}

static void
clientSaveCompleteCB(SmcConn smcConn, SmPointer data)
{
	if (saving_yourself) {
		saving_yourself = False;
		gtk_main_quit();
	}

}

/** @brief shutdown cancelled
  *
  * The session manager sends a "Shutdown Cancelled" message when the user
  * cancelled the shutdown during an interaction (see Section 5.5, "Interacting
  * With the User").  The client can now continue as if the shutdown had never
  * happended.  If the client has not called SmcSaveYourselfDone() yet, it can
  * either abort the save and then send SmcSaveYourselfDone() with the success
  * argument set to False or it can continue with the save and then call
  * SmcSaveYourselfDone() with the success argument set to reflect the outcome
  * of the save.
  */
static void
clientShutdownCancelledCB(SmcConn smcConn, SmPointer data)
{
	shutting_down = False;
	gtk_main_quit();
}

/* *INDENT-OFF* */
static unsigned long clientCBMask =
	SmcSaveYourselfProcMask |
	SmcDieProcMask |
	SmcSaveCompleteProcMask |
	SmcShutdownCancelledProcMask;

static SmcCallbacks clientCBs = {
	.save_yourself = {
		.callback = &clientSaveYourselfCB,
		.client_data = NULL,
	},
	.die = {
		.callback = &clientDieCB,
		.client_data = NULL,
	},
	.save_complete = {
		.callback = &clientSaveCompleteCB,
		.client_data = NULL,
	},
	.shutdown_cancelled = {
		.callback = &clientShutdownCancelledCB,
		.client_data = NULL,
	},
};
/* *INDENT-ON* */

static gboolean
on_ifd_watch(GIOChannel *chan, GIOCondition cond, gpointer data)
{
	SmcConn smcConn = data;
	IceConn iceConn = SmcGetIceConnection(smcConn);

	if (cond & (G_IO_NVAL | G_IO_HUP | G_IO_ERR)) {
		EPRINTF("poll failed: %s %s %s\n",
			(cond & G_IO_NVAL) ? "NVAL" : "",
			(cond & G_IO_HUP) ? "HUP" : "", (cond & G_IO_ERR) ? "ERR" : "");
		return G_SOURCE_REMOVE;	/* remove event source */
	} else if (cond & (G_IO_IN | G_IO_PRI)) {
		IceProcessMessages(iceConn, NULL, NULL);
	}
	return G_SOURCE_CONTINUE;	/* keep event source */
}

static void
init_smclient(void)
{
	char err[256] = { 0, };
	GIOChannel *chan;
	int ifd, mask = G_IO_IN | G_IO_ERR | G_IO_HUP | G_IO_PRI;
	char *env;
	IceConn iceConn;

	if (!(env = getenv("SESSION_MANAGER"))) {
		if (options.clientId)
			EPRINTF("clientId provided but no SESSION_MANAGER\n");
		return;
	}
	smcConn = SmcOpenConnection(env, NULL, SmProtoMajor, SmProtoMinor,
				    clientCBMask, &clientCBs, options.clientId,
				    &options.clientId, sizeof(err), err);
	if (!smcConn) {
		EPRINTF("SmcOpenConnection: %s\n", err);
		return;
	}
	iceConn = SmcGetIceConnection(smcConn);
	ifd = IceConnectionNumber(iceConn);
	chan = g_io_channel_unix_new(ifd);
	g_io_add_watch(chan, mask, on_ifd_watch, smcConn);
}

/*
 *  This startup function starts up the X11 protocol connection and initializes GTK+.  Note that the
 *  program can still be run from a console, in which case the "DISPLAY" environment variables should
 *  not be defined: in which case, we will not start up X11 at all.
 */
static void
startup(int argc, char *argv[], Command command)
{
	static const char *suffix = "/.gtkrc-2.0.xde";
	const char *home;
	GdkAtom atom;
	GdkEventMask mask;
	GdkDisplay *disp;
	GdkScreen *scrn;
	GdkWindow *root;
	Display *dpy;
	char *file;
	int len;

	/* We can start session management without a display; however, we then need to
	   run a GLIB event loop instead of a GTK event loop.  */
	init_smclient();

	/* do not start up X11 connection unless DISPLAY is defined */
	if (!options.display) {
		loop = g_main_loop_new(NULL, FALSE);
		return;
	}

	home = getenv("HOME") ? : ".";
	len = strlen(home) + strlen(suffix) + 1;
	file = calloc(len + 1, sizeof(*file));
	strncpy(file, home, len);
	strncat(file, suffix, len);
	gtk_rc_add_default_file(file);
	free(file);

	gtk_init(&argc, &argv);

	disp = gdk_display_get_default();
	dpy = GDK_DISPLAY_XDISPLAY(disp);

	atom = gdk_atom_intern_static_string("_XDE_ICON_THEME_NAME");
	_XA_XDE_ICON_THEME_NAME = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_THEME_NAME");
	_XA_XDE_THEME_NAME = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_CLASS");
	_XA_XDE_WM_CLASS = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_CMDLINE");
	_XA_XDE_WM_CMDLINE = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_COMMAND");
	_XA_XDE_WM_COMMAND = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_ETCDIR");
	_XA_XDE_WM_ETCDIR = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_HOST");
	_XA_XDE_WM_HOST = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_HOSTNAME");
	_XA_XDE_WM_HOSTNAME = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_ICCCM_SUPPORT");
	_XA_XDE_WM_ICCCM_SUPPORT = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_ICON");
	_XA_XDE_WM_ICON = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_ICONTHEME");
	_XA_XDE_WM_ICONTHEME = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_INFO");
	_XA_XDE_WM_INFO = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_MENU");
	_XA_XDE_WM_MENU = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_NAME");
	_XA_XDE_WM_NAME = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_NETWM_SUPPORT");
	_XA_XDE_WM_NETWM_SUPPORT = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_PID");
	_XA_XDE_WM_PID = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_PRVDIR");
	_XA_XDE_WM_PRVDIR = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_RCFILE");
	_XA_XDE_WM_RCFILE = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_REDIR_SUPPORT");
	_XA_XDE_WM_REDIR_SUPPORT = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_STYLE");
	_XA_XDE_WM_STYLE = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_STYLENAME");
	_XA_XDE_WM_STYLENAME = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_SYSDIR");
	_XA_XDE_WM_SYSDIR = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_THEME");
	_XA_XDE_WM_THEME = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_THEMEFILE");
	_XA_XDE_WM_THEMEFILE = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_USRDIR");
	_XA_XDE_WM_USRDIR = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_WM_VERSION");
	_XA_XDE_WM_VERSION = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_GTK_READ_RCFILES");
	_XA_GTK_READ_RCFILES = gdk_x11_atom_to_xatom_for_display(disp, atom);
	gdk_display_add_client_message_filter(disp, atom, client_handler, dpy);

	atom = gdk_atom_intern_static_string("MANAGER");
	_XA_MANAGER = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_UP_APPLET_REFRESH");
	_XA_XDE_UP_APPLET_REFRESH = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_UP_APPLET_RESTART");
	_XA_XDE_UP_APPLET_RESTART = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_UP_APPLET_POPMENU");
	_XA_XDE_UP_APPLET_POPMENU = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_UP_APPLET_REQUEST");
	_XA_XDE_UP_APPLET_REQUEST = gdk_x11_atom_to_xatom_for_display(disp, atom);

	scrn = gdk_display_get_default_screen(disp);
	root = gdk_screen_get_root_window(scrn);
	mask = gdk_window_get_events(root);
	mask |= GDK_PROPERTY_CHANGE_MASK | GDK_STRUCTURE_MASK | GDK_SUBSTRUCTURE_MASK;
	gdk_window_set_events(root, mask);

	wnck_set_client_type(WNCK_CLIENT_TYPE_PAGER);
}

static void
do_run(int argc, char *argv[], Bool replace)
{
	setup_x11(replace);
	gtk_main();
}

static void
do_refresh(int argc, char *argv[])
{
	char selection[64] = { 0, };
	GdkDisplay *disp;
	Display *dpy;
	int s, nscr;
	Atom atom;
	Window owner, gotone = None;

	if (!options.display) {
		EPRINTF("%s: need display to refresh instance\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	disp = gdk_display_get_default();
	nscr = gdk_display_get_n_screens(disp);

	dpy = GDK_DISPLAY_XDISPLAY(disp);

	for (s = 0; s < nscr; s++) {
		snprintf(selection, sizeof(selection), XA_SELECTION_NAME, s);
		atom = XInternAtom(dpy, selection, False);
		if ((owner = XGetSelectionOwner(dpy, atom)) && gotone != owner) {
			XEvent ev;

			ev.xclient.type = ClientMessage;
			ev.xclient.serial = 0;
			ev.xclient.send_event = False;
			ev.xclient.display = dpy;
			ev.xclient.window = RootWindow(dpy, s);
			ev.xclient.message_type = _XA_XDE_UP_APPLET_REFRESH;
			ev.xclient.format = 32;
			ev.xclient.data.l[0] = CurrentTime;
			ev.xclient.data.l[1] = atom;

			XSendEvent(dpy, owner, False, StructureNotifyMask, &ev);
			XFlush(dpy);

			gotone = owner;
			break;
		}
	}
	if (!gotone) {
		EPRINTF("%s: need running instance to refresh\n", argv[0]);
		exit(EXIT_FAILURE);
	}
}

static void
do_restart(int argc, char *argv[])
{
	char selection[64] = { 0, };
	GdkDisplay *disp;
	Display *dpy;
	int s, nscr;
	Atom atom;
	Window owner, gotone = None;

	if (!options.display) {
		EPRINTF("%s: need display to restart instance\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	disp = gdk_display_get_default();
	nscr = gdk_display_get_n_screens(disp);

	dpy = GDK_DISPLAY_XDISPLAY(disp);

	for (s = 0; s < nscr; s++) {
		snprintf(selection, sizeof(selection), XA_SELECTION_NAME, s);
		atom = XInternAtom(dpy, selection, False);
		if ((owner = XGetSelectionOwner(dpy, atom)) && gotone != owner) {
			XEvent ev;

			ev.xclient.type = ClientMessage;
			ev.xclient.serial = 0;
			ev.xclient.send_event = False;
			ev.xclient.display = dpy;
			ev.xclient.window = RootWindow(dpy, s);
			ev.xclient.message_type = _XA_XDE_UP_APPLET_RESTART;
			ev.xclient.format = 32;
			ev.xclient.data.l[0] = CurrentTime;
			ev.xclient.data.l[1] = atom;

			XSendEvent(dpy, owner, False, StructureNotifyMask, &ev);
			XFlush(dpy);

			gotone = owner;
			break;
		}
	}
	if (!gotone) {
		EPRINTF("%s: need running instance to restart\n", argv[0]);
		exit(EXIT_FAILURE);
	}
}

static void
do_quit(int argc, char *argv[])
{
	get_selection(True, None);
}

static void
copying(int argc, char *argv[])
{
	if (!options.output && !options.debug)
		return;
	(void) fprintf(stdout, "\
--------------------------------------------------------------------------------\n\
%1$s\n\
--------------------------------------------------------------------------------\n\
Copyright (c) 2008-2018  Monavacon Limited <http://www.monavacon.com/>\n\
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
--------------------------------------------------------------------------------\n\
This program is free software: you can  redistribute it  and/or modify  it under\n\
the terms of the  GNU Affero  General  Public  License  as published by the Free\n\
Software Foundation, version 3 of the license.\n\
\n\
This program is distributed in the hope that it will  be useful, but WITHOUT ANY\n\
WARRANTY; without even  the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.\n\
\n\
You should have received a copy of the  GNU Affero General Public License  along\n\
with this program.   If not, see <http://www.gnu.org/licenses/>, or write to the\n\
Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
--------------------------------------------------------------------------------\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the U.S. Government (\"Government\"), the following provisions apply to you. If\n\
the Software is supplied by the Department of Defense (\"DoD\"), it is classified\n\
as \"Commercial  Computer  Software\"  under  paragraph  252.227-7014  of the  DoD\n\
Supplement  to the  Federal Acquisition Regulations  (\"DFARS\") (or any successor\n\
regulations) and the  Government  is acquiring  only the  license rights granted\n\
herein (the license rights customarily provided to non-Government users). If the\n\
Software is supplied to any unit or agency of the Government  other than DoD, it\n\
is  classified as  \"Restricted Computer Software\" and the Government's rights in\n\
the Software  are defined  in  paragraph 52.227-19  of the  Federal  Acquisition\n\
Regulations (\"FAR\")  (or any successor regulations) or, in the cases of NASA, in\n\
paragraph  18.52.227-86 of  the  NASA  Supplement  to the FAR (or any  successor\n\
regulations).\n\
--------------------------------------------------------------------------------\n\
", NAME " " VERSION);
}

static void
version(int argc, char *argv[])
{
	if (!options.output && !options.debug)
		return;
	(void) fprintf(stdout, "\
%1$s (OpenSS7 %2$s) %3$s\n\
Written by Brian Bidulock.\n\
\n\
Copyright (c) 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018  Monavacon Limited.\n\
Copyright (c) 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009  OpenSS7 Corporation.\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock.\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 under GNU Affero General Public License Version 3,\n\
with conditions, incorporated herein by reference.\n\
\n\
See `%1$s --copying' for copying permissions.\n\
", NAME, PACKAGE, VERSION);
}

static void
usage(int argc, char *argv[])
{
	if (!options.output && !options.debug)
		return;
	(void) fprintf(stderr, "\
Usage:\n\
    %1$s [options]\n\
    %1$s {-m|--monitor} [options]\n\
    %1$s {-R|--replace} [options]\n\
    %1$s {-q|--quit} [options]\n\
    %1$s {-h|--help} [options]\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
", argv[0]);
}

const char *
show_bool(Bool boolval)
{
	if (boolval)
		return ("true");
	return ("false");
}

static void
help(int argc, char *argv[])
{
	if (!options.output && !options.debug)
		return;
	/* *INDENT-OFF* */
	(void) fprintf(stdout, "\
Usage:\n\
    %1$s [options]\n\
    %1$s {-m|--monitor} [options]\n\
    %1$s {-R|--replace} [options]\n\
    %1$s {-E|--refresh} [options]\n\
    %1$s {-S|--restart} [options]\n\
    %1$s {-q|--quit} [options]\n\
    %1$s {-h|--help} [options]\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Command options:\n\
    -m, --monitor\n\
        generate a menu and monitor for changes and requests\n\
    -R, --replace\n\
        replace a running instance\n\
    -E, --refresh\n\
        ask a running instance to refresh the menus\n\
    -S, --restart\n\
        ask a running instance to reexecute itself\n\
    -q, --quit\n\
        ask a running instance to quit\n\
    -h, --help, -?, --?\n\
        print this usage information and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
General options:\n\
    --display DISPLAY\n\
        specify the X11 display [default: %2$s]\n\
    --screen SCREEN\n\
        specify the X11 scrfeen [default: %3$d]\n\
    -e, --die-on-error\n\
        abort on error [default: %4$s]\n\
    --notray\n\
        do not install a system tray icon [default: %5$s]\n\
    --nodock\n\
        do not install a withdrawn dock app [default: %6$s]\n\
    -D, --debug [LEVEL]\n\
        increment or set debug LEVEL [default: %7$d]\n\
    -v, --verbose [LEVEL]\n\
        increment or set output verbosity LEVEL [default: %8$d]\n\
        this option may be repeated.\n\
", argv[0]
	, defaults.display
	, defaults.screen
	, show_bool(defaults.dieonerr)
	, show_bool(!defaults.tray)
	, show_bool(!defaults.dock)
	, defaults.debug
	, defaults.output
);
	/* *INDENT-ON* */
}

static void
get_defaults()
{
}

/*
 * Set options in the "defaults" structure.  These "defaults" are determined by preset defaults,
 * environment variables and other startup information, but not information from the X Server.  All
 * options are set in this way, only the ones that depend on environment variables or other startup
 * information.
 */
static void
set_defaults(void)
{
	char *env;

	if ((env = getenv("DISPLAY"))) {
		free(options.display);
		defaults.display = options.display = strdup(env);
	}
}

int
main(int argc, char *argv[])
{
	Command command = CommandDefault;
	char *p;

	setlocale(LC_ALL, "");
	set_defaults();

	saveArgc = argc;
	saveArgv = argv;

	if ((p = strstr(argv[0], "-menugen")) && !p[8])
		defaults.command = options.command = CommandMenugen;
	else if ((p = strstr(argv[0], "-popmenu")) && !p[6])
		defaults.command = options.command = CommandPopMenu;
	else if ((p = strstr(argv[0], "-monitor")) && !p[8])
		defaults.command = options.command = CommandMonitor;
	else if ((p = strstr(argv[0], "-replace")) && !p[8])
		defaults.command = options.command = CommandReplace;
	else if ((p = strstr(argv[0], "-refresh")) && !p[8])
		defaults.command = options.command = CommandRefresh;
	else if ((p = strstr(argv[0], "-restart")) && !p[8])
		defaults.command = options.command = CommandRestart;
	else if ((p = strstr(argv[0], "-quit")) && !p[5])
		defaults.command = options.command = CommandQuit;

	while (1) {
		int c, val;
		char *endptr = NULL;

#ifdef _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"wmname",	required_argument,	NULL,	'w'},
			{"format",	required_argument,	NULL,	'f'},
			{"fullmenu",	no_argument,		NULL,	'F'},
			{"nofullmenu",	no_argument,		NULL,	'N'},
			{"desktop",	required_argument,	NULL,	'd'},
			{"charset",	required_argument,	NULL,	'c'},
			{"language",	required_argument,	NULL,	'l'},
			{"root-menu",	required_argument,	NULL,	'r'},
			{"output",	optional_argument,	NULL,	'o'},
			{"noicons",	no_argument,		NULL,	'n'},
			{"theme",	required_argument,	NULL,	't'},
			{"launch",	no_argument,		NULL,	'L'},
			{"nolaunch",	no_argument,		NULL,	'0'},
			{"style",	required_argument,	NULL,	's'},
			{"menu",	required_argument,	NULL,	'M'},

			{"button",	required_argument,	NULL,	'b'},
			{"keypress",	optional_argument,	NULL,	'k'},
			{"timestamp",	required_argument,	NULL,	'T'},
			{"which",	required_argument,	NULL,	'i'},
			{"where",	required_argument,	NULL,	'W'},

			{"display",	required_argument,	NULL,	 1 },
			{"screen",	required_argument,	NULL,	 4 },
			{"die-on-error",no_argument,		NULL,	'e'},
			{"notray",	no_argument,		NULL,	 2 },
			{"nodock",	no_argument,		NULL,	 5 },
			{"nogenerate",	no_argument,		NULL,	 3 },
			{"verbose",	optional_argument,	NULL,	'v'},
			{"debug",	optional_argument,	NULL,	'D'},

			{"menugen",	no_argument,		NULL,	'G'},
			{"popmenu",	no_argument,		NULL,	'P'},
			{"monitor",	no_argument,		NULL,	'm'},
			{"refresh",	no_argument,		NULL,	'E'},
			{"restart",	no_argument,		NULL,	'S'},
			{"replace",	no_argument,		NULL,	'R'},
			{"quit",	no_argument,		NULL,	'q'},

			{"excluded",	no_argument,		NULL,	 10},
			{"nodisplay",	no_argument,		NULL,	 11},
			{"unallocated",	no_argument,		NULL,	 12},
			{"empty",	no_argument,		NULL,	 13},
			{"separators",	no_argument,		NULL,	 14},
			{"sort",	no_argument,		NULL,	 15},
			{"tooltips",	no_argument,		NULL,	 16},
			{"actions",	no_argument,		NULL,	 17},

			{"help",	no_argument,		NULL,	'h'},
			{"version",	no_argument,		NULL,	'V'},
			{"copying",	no_argument,		NULL,	'C'},
			{"?",		no_argument,		NULL,	'H'},
			{ 0, }
		};
		/* *INDENT-ON* */

		c = getopt_long_only(argc, argv,
				     "w:f:FNd:c:l:r:o::nt:L0s:M:b:k::T:W:ev::D::GPmFSRqhVCH?",
				     long_options, &option_index);
#else
		c = getopt(argc, argv, "w:f:FNd:c:l:r:o:nt:L0s:M:b:k:T:W:ev:D:GPmFSRqhVCH?");
#endif
		if (c == -1) {
			DPRINTF(1, "%s: done options processing\n", argv[0]);
			break;
		}
		switch (c) {
		case 0:
			goto bad_usage;


		case 1:	/* --display DISPLAY */
			free(options.display);
			defaults.display = options.display = strdup(optarg);
			break;
		case 4:	/* --screen SCREEN */
			options.screen = atoi(optarg);
			break;
		case 2:	/* --notray */
			options.tray = False;
			break;
		case 5: /* --nodock */
			options.dock = False;
			break;

		case 'm':	/* -m, --monitor */
			if (options.command != CommandDefault)
				goto bad_option;
			if (command == CommandDefault) {
				command = CommandMonitor;
				DPRINTF(1, "Setting command to CommandMonitor\n");
			}
			defaults.command = options.command = CommandMonitor;
			break;
		case 'R':	/* -R, --replace */
			if (options.command != CommandDefault)
				goto bad_option;
			if (command == CommandDefault) {
				DPRINTF(1, "Setting command to CommandReplace\n");
				command = CommandReplace;
			}
			defaults.command = options.command = CommandReplace;
			break;
		case 'E':	/* -F, --refresh */
			if (options.command != CommandDefault)
				goto bad_option;
			if (command == CommandDefault) {
				DPRINTF(1, "Setting command to CommandRefresh\n");
				command = CommandRefresh;
			}
			defaults.command = options.command = CommandRefresh;
			break;
		case 'S':	/* -S, --restart */
			if (options.command != CommandDefault)
				goto bad_option;
			if (command == CommandDefault) {
				DPRINTF(1, "Setting command to CommandRestart\n");
				command = CommandRestart;
			}
			defaults.command = options.command = CommandRestart;
			break;
		case 'q':	/* -q, --quit */
			if (options.command != CommandDefault)
				goto bad_option;
			if (command == CommandDefault) {
				DPRINTF(1, "Setting command to CommandQuit\n");
				command = CommandQuit;
			}
			defaults.command = options.command = CommandQuit;
			break;

		case 'D':	/* -D, --debug [LEVEL] */
			DPRINTF(1, "%s: increasing debug verbosity\n", argv[0]);
			if (optarg == NULL) {
				defaults.debug = options.debug = options.debug + 1;
				break;
			}
			if ((val = strtol(optarg, &endptr, 0)) < 0)
				goto bad_option;
			if (endptr && *endptr)
				goto bad_option;
			defaults.debug = options.debug = val;
			break;
		case 'v':	/* -v, --verbose [LEVEL] */
			DPRINTF(1, "%s: increasing output verbosity\n", argv[0]);
			if (optarg == NULL) {
				defaults.output = options.output = options.output + 1;
				break;
			}
			if ((val = strtol(optarg, &endptr, 0)) < 0)
				goto bad_option;
			if (endptr && *endptr)
				goto bad_option;
			defaults.output = options.output = val;
			break;
		case 'h':	/* -h, --help */
		case 'H':	/* -H, --? */
			DPRINTF(1, "Setting command to CommandHelp\n");
			command = CommandHelp;
			break;
		case 'V':
			DPRINTF(1, "Setting command to CommandVersion\n");
			command = CommandVersion;
			break;
		case 'C':	/* -C, --copying */
			DPRINTF(1, "Setting command to CommandCopying\n");
			command = CommandCopying;
			break;
		case '?':
		default:
		      bad_option:
			optind--;
			goto bad_nonopt;
		      bad_nonopt:
			if (options.output || options.debug) {
				if (optind < argc) {
					EPRINTF("%s: syntax error near '", argv[0]);
					while (optind < argc) {
						fprintf(stderr, "%s", argv[optind++]);
						fprintf(stderr, "%s", (optind < argc) ? " " : "");
					}
					fprintf(stderr, "'\n");
				} else {
					EPRINTF("%s: missing option or argument", argv[0]);
					fprintf(stderr, "\n");
				}
				fflush(stderr);
			      bad_usage:
				usage(argc, argv);
			}
			exit(EXIT_SYNTAXERR);
		}
	}
	DPRINTF(1, "%s: option index = %d\n", argv[0], optind);
	DPRINTF(1, "%s: option count = %d\n", argv[0], argc);
	if (optind < argc) {
		EPRINTF("%s: excess non-option arguments near '", argv[0]);
		while (optind < argc) {
			fprintf(stderr, "%s", argv[optind++]);
			fprintf(stderr, "%s", (optind < argc) ? " " : "");
		}
		fprintf(stderr, "'\n");
		usage(argc, argv);
		exit(EXIT_SYNTAXERR);
	}
	startup(argc, argv, command);
	get_defaults();

	switch (command) {
	default:
	case CommandDefault:
		defaults.command = options.command = CommandMenugen;
	case CommandMonitor:
		DPRINTF(1, "%s: running a new instance\n", argv[0]);
		do_run(argc, argv, False);
		break;
	case CommandReplace:
		DPRINTF(1, "%s: replacing existing instance\n", argv[0]);
		do_run(argc, argv, True);
		break;
	case CommandRefresh:
		DPRINTF(1, "%s: asking existing instance to refresh\n", argv[0]);
		do_refresh(argc, argv);
		break;
	case CommandRestart:
		DPRINTF(1, "%s: asking existing instance to restart\n", argv[0]);
		do_restart(argc, argv);
		break;
	case CommandQuit:
		if (!options.display) {
			EPRINTF("%s: cannot ask instance to quit without DISPLAY\n", argv[0]);
			exit(EXIT_FAILURE);
		}
		DPRINTF(1, "%s: asking existing instance to quit\n", argv[0]);
		do_quit(argc, argv);
		break;
	case CommandHelp:
		DPRINTF(1, "%s: printing help message\n", argv[0]);
		help(argc, argv);
		break;
	case CommandVersion:
		DPRINTF(1, "%s: printing version message\n", argv[0]);
		version(argc, argv);
		break;
	case CommandCopying:
		DPRINTF(1, "%s: printing copying message\n", argv[0]);
		copying(argc, argv);
		break;
	}
	exit(EXIT_SUCCESS);
}

// vim: set sw=8 tw=100 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
