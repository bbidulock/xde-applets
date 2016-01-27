
AM_CFLAGS = $(X11_CFLAGS) $(XEXT_CFLAGS) $(XSCRNSAVER_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(CAIRO_CFLAGS) $(GIO_UNIX_CFLAGS) $(GDK_CFLAGS) $(GTK_CFLAGS) $(WNCK_CFLAGS)
AM_LDFLAGS = $(WNCK_LIBS) $(GTK_LIBS) $(GDK_LIBS) $(GIO_UNIX_LIBS) $(CAIRO_LIBS) $(XINERAMA_LIBS) $(XRANDR_LIBS) $(XSCRNSAVER_LIBS) $(XEXT_LIBS) $(X11_LIBS)

bin_PROGRAMS = \
	av-applet \
	bt-applet \
	dh-applet \
	hal-applet \
	pk-applet \
	sd-applet \
	wl-applet

av_applet_CPPFLAGS = -DNAME=\"av-applet\"
av_applet_SOURCES = av-applet.c
av_applet_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS)
av_applet_LDFLAGS = $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

bt_applet_CPPFLAGS = -DNAME=\"bt-applet\"
bt_applet_SOURCES = bt-applet.c
bt_applet_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS)
bt_applet_LDFLAGS = $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

dh_applet_CPPFLAGS = -DNAME=\"dh-applet\"
dh_applet_SOURCES = dh-applet.c
dh_applet_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS)
dh_applet_LDFLAGS = $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

hal_applet_CPPFLAGS = -DNAME=\"hal-applet\"
hal_applet_SOURCES = hal-applet.c
hal_applet_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS)
hal_applet_LDFLAGS = $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

pk_applet_CPPFLAGS = -DNAME=\"pk-applet\"
pk_applet_SOURCES = pk-applet.c
pk_applet_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS)
pk_applet_LDFLAGS = $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

sd_applet_CPPFLAGS = -DNAME=\"sd-applet\"
sd_applet_SOURCES = sd-applet.c
sd_applet_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS)
sd_applet_LDFLAGS = $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

wl_applet_CPPFLAGS = -DNAME=\"wl-applet\"
wl_applet_SOURCES = wl-applet.c
wl_applet_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS)
wl_applet_LDFLAGS = $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

