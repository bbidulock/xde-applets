/*****************************************************************************

 Copyright (c) 2008-2016  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
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

#ifdef HAVE_CONFIG_H
#include "autoconf.h"
#endif

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif

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
#include <pwd.h>
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
#include <glib.h>
#include <gdk/gdkx.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>
#include <cairo.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

#include <pwd.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#define XPRINTF(args...) do { } while (0)
#define OPRINTF(args...) do { if (options.output > 1) { \
	fprintf(stderr, "I: "); \
	fprintf(stderr, args); \
	fflush(stderr); } } while (0)
#define DPRINTF(args...) do { if (options.debug) { \
	fprintf(stderr, "D: %s +%d %s(): ", __FILE__, __LINE__, __func__); \
	fprintf(stderr, args); \
	fflush(stderr); } } while (0)
#define EPRINTF(args...) do { \
	fprintf(stderr, "E: %s +%d %s(): ", __FILE__, __LINE__, __func__); \
	fprintf(stderr, args); \
	fflush(stderr);   } while (0)
#define WPRINTF(args...) do { \
	fprintf(stderr, "W: %s +%d %s(): ", __FILE__, __LINE__, __func__); \
	fprintf(stderr, args); \
	fflush(stderr);   } while (0)
#define DPRINT() do { if (options.debug) { \
	fprintf(stderr, "D: %s +%d %s()\n", __FILE__, __LINE__, __func__); \
	fflush(stderr); } } while (0)

#undef EXIT_SUCCESS
#undef EXIT_FAILURE
#undef EXIT_SYNTAXERR

#define EXIT_SUCCESS	0
#define EXIT_FAILURE	1
#define EXIT_SYNTAXERR	2

#define GTK_EVENT_STOP		TRUE
#define GTK_EVENT_PROPAGATE	FALSE

#define XA_SELECTION_NAME	"_XDE_LM_APPLET_S%d"

#define LOGO_NAME		"network-wired"

SmcConn smcConn = NULL;

int saveArgc;
char **saveArgv;

int cmdArgc;
char **cmdArgv;

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

Atom _XA_XDE_LM_APPLET_REFRESH;
Atom _XA_XDE_LM_APPLET_RESTART;
Atom _XA_XDE_LM_APPLET_POPMENU;
Atom _XA_XDE_LM_APPLET_REQUEST;

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
			      "comments", "An LM sensors system tray icon.",
			      "copyright", "Copyright (c) 2013, 2014, 2015, 2016  OpenSS7 Corporation",
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

	DPRINTF("%s: restarting the applet\n", NAME);
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
			DPRINTF("No owner for %s\n", selection);
		if ((owner && replace) || (!owner && selwin)) {
			DPRINTF("Setting owner of %s to 0x%08lx from 0x%08lx\n", selection, selwin,
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
				DPRINTF("%s: replacing running instance\n", NAME);
			else
				DPRINTF("%s: quitting running instance\n", NAME);
		} else {
			if (selwin)
				DPRINTF("%s: no running instance to replace\n", NAME);
			else
				DPRINTF("%s: no running instance to quit\n", NAME);
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
		DPRINTF("%s: not replacing running instance\n", NAME);
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
	DPRINTF("window manager is '%s'\n", xscr->wmname);
	DPRINTF("window manager is %s\n", xscr->goodwm ? "usable" : "unusable");
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
			DPRINTF("could not get text list for property\n");
		if (xtp.value)
			XFree(xtp.value);
	} else
		DPRINTF("could not get %s for root 0x%lx\n", XGetAtomName(dpy, prop), root);
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
		DPRINTF("New theme is %s\n", xscr->theme);
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
			DPRINTF("could not get text list for property\n");
		if (xtp.value)
			XFree(xtp.value);
	} else
		DPRINTF("could not get %s for root 0x%lx\n", XGetAtomName(dpy, prop), root);
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
		DPRINTF("New icon theme is %s\n", xscr->itheme);
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

	DPRINTF("getting default GDK display\n");
	disp = gdk_display_get_default();
	DPRINTF("getting default display\n");
	dpy = GDK_DISPLAY_XDISPLAY(disp);
	DPRINTF("getting default GDK screen\n");
	scrn = gdk_display_get_default_screen(disp);
	DPRINTF("getting default GDK root window\n");
	root = gdk_screen_get_root_window(scrn);

	DPRINTF("creating select window\n");
	selwin = XCreateSimpleWindow(dpy, GDK_WINDOW_XID(root), 0, 0, 1, 1, 0, 0, 0);

	DPRINTF("checking for selection\n");
	if ((owner = get_selection(replace, selwin))) {
		if (!replace) {
			XDestroyWindow(dpy, selwin);
			EPRINTF("%s: instance 0x%08lx is already running\n", NAME, owner);
			exit(EXIT_FAILURE);
		}
	}
	DPRINTF("selecting inputs on 0x%08lx\n", selwin);
	XSelectInput(dpy, selwin,
		     StructureNotifyMask | SubstructureNotifyMask | PropertyChangeMask);

	DPRINTF("getting number of screens\n");
	nscr = gdk_display_get_n_screens(disp);
	DPRINTF("allocating %d screen structures\n", nscr);
	screens = calloc(nscr, sizeof(*screens));

	DPRINTF("getting GDK window for 0x%08lx\n", selwin);
	sel = gdk_x11_window_foreign_new_for_display(disp, selwin);
	DPRINTF("adding a filter for the select window\n");
	gdk_window_add_filter(sel, selwin_handler, screens);

	DPRINTF("initializing %d screens\n", nscr);
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
		init_wnck(xscr);
		update_theme(xscr, None);
		update_icon_theme(xscr, None);
		if (options.tray)
			init_statusicon(xscr);
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
	} else if (xscr && xev->xclient.message_type == _XA_XDE_LM_APPLET_REFRESH) {
		// set_flags(xev->xclient.data.l[2]);
		// set_word1(xev->xclient.data.l[3]);
		// set_word2(xev->xclient.data.l[4]);
		applet_refresh(xscr);
		return GDK_FILTER_REMOVE;
	} else if (xscr && xev->xclient.message_type == _XA_XDE_LM_APPLET_RESTART) {
		// set_flags(xev->xclient.data.l[2]);
		// set_word1(xev->xclient.data.l[3]);
		// set_word2(xev->xclient.data.l[4]);
		applet_restart();
		return GDK_FILTER_REMOVE;
	} else if (xscr && xev->xclient.message_type == _XA_XDE_LM_APPLET_POPMENU) {
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

	DPRINT();
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
	DPRINT();
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
		DPRINTF("selection cleared, exiting\n");
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
	DPRINT();
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
	DPRINT();
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

	DPRINT();
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

	DPRINT();
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

	atom = gdk_atom_intern_static_string("_XDE_LM_APPLET_REFRESH");
	_XA_XDE_LM_APPLET_REFRESH = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_LM_APPLET_RESTART");
	_XA_XDE_LM_APPLET_RESTART = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_LM_APPLET_POPMENU");
	_XA_XDE_LM_APPLET_POPMENU = gdk_x11_atom_to_xatom_for_display(disp, atom);

	atom = gdk_atom_intern_static_string("_XDE_LM_APPLET_REQUEST");
	_XA_XDE_LM_APPLET_REQUEST = gdk_x11_atom_to_xatom_for_display(disp, atom);

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
			ev.xclient.message_type = _XA_XDE_LM_APPLET_REFRESH;
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
			ev.xclient.message_type = _XA_XDE_LM_APPLET_RESTART;
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
Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>\n\
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
Copyright (c) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016  Monavacon Limited.\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008  OpenSS7 Corporation.\n\
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
    -D, --debug [LEVEL]\n\
        increment or set debug LEVEL [default: %6$d]\n\
    -v, --verbose [LEVEL]\n\
        increment or set output verbosity LEVEL [default: %7$d]\n\
        this option may be repeated.\n\
", argv[0]
	, defaults.display
	, defaults.screen
	, show_bool(defaults.dieonerr)
	, show_bool(!defaults.tray)
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
			DPRINTF("%s: done options processing\n", argv[0]);
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

		case 'm':	/* -m, --monitor */
			if (options.command != CommandDefault)
				goto bad_option;
			if (command == CommandDefault) {
				command = CommandMonitor;
				DPRINTF("Setting command to CommandMonitor\n");
			}
			defaults.command = options.command = CommandMonitor;
			break;
		case 'R':	/* -R, --replace */
			if (options.command != CommandDefault)
				goto bad_option;
			if (command == CommandDefault) {
				DPRINTF("Setting command to CommandReplace\n");
				command = CommandReplace;
			}
			defaults.command = options.command = CommandReplace;
			break;
		case 'E':	/* -F, --refresh */
			if (options.command != CommandDefault)
				goto bad_option;
			if (command == CommandDefault) {
				DPRINTF("Setting command to CommandRefresh\n");
				command = CommandRefresh;
			}
			defaults.command = options.command = CommandRefresh;
			break;
		case 'S':	/* -S, --restart */
			if (options.command != CommandDefault)
				goto bad_option;
			if (command == CommandDefault) {
				DPRINTF("Setting command to CommandRestart\n");
				command = CommandRestart;
			}
			defaults.command = options.command = CommandRestart;
			break;
		case 'q':	/* -q, --quit */
			if (options.command != CommandDefault)
				goto bad_option;
			if (command == CommandDefault) {
				DPRINTF("Setting command to CommandQuit\n");
				command = CommandQuit;
			}
			defaults.command = options.command = CommandQuit;
			break;

		case 'D':	/* -D, --debug [LEVEL] */
			DPRINTF("%s: increasing debug verbosity\n", argv[0]);
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
			DPRINTF("%s: increasing output verbosity\n", argv[0]);
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
			DPRINTF("Setting command to CommandHelp\n");
			command = CommandHelp;
			break;
		case 'V':
			DPRINTF("Setting command to CommandVersion\n");
			command = CommandVersion;
			break;
		case 'C':	/* -C, --copying */
			DPRINTF("Setting command to CommandCopying\n");
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
	DPRINTF("%s: option index = %d\n", argv[0], optind);
	DPRINTF("%s: option count = %d\n", argv[0], argc);
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
		DPRINTF("%s: running a new instance\n", argv[0]);
		do_run(argc, argv, False);
		break;
	case CommandReplace:
		DPRINTF("%s: replacing existing instance\n", argv[0]);
		do_run(argc, argv, True);
		break;
	case CommandRefresh:
		DPRINTF("%s: asking existing instance to refresh\n", argv[0]);
		do_refresh(argc, argv);
		break;
	case CommandRestart:
		DPRINTF("%s: asking existing instance to restart\n", argv[0]);
		do_restart(argc, argv);
		break;
	case CommandQuit:
		if (!options.display) {
			EPRINTF("%s: cannot ask instance to quit without DISPLAY\n", argv[0]);
			exit(EXIT_FAILURE);
		}
		DPRINTF("%s: asking existing instance to quit\n", argv[0]);
		do_quit(argc, argv);
		break;
	case CommandHelp:
		DPRINTF("%s: printing help message\n", argv[0]);
		help(argc, argv);
		break;
	case CommandVersion:
		DPRINTF("%s: printing version message\n", argv[0]);
		version(argc, argv);
		break;
	case CommandCopying:
		DPRINTF("%s: printing copying message\n", argv[0]);
		copying(argc, argv);
		break;
	}
	exit(EXIT_SUCCESS);
}

// vim: set sw=8 tw=100 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker: