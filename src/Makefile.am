
AM_CFLAGS = $(X11_CFLAGS) $(XEXT_CFLAGS) $(XSCRNSAVER_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(CAIRO_CFLAGS) $(GIO_UNIX_CFLAGS) $(GDK_CFLAGS) $(GTK_CFLAGS) $(NOTIFY_CFLAGS) $(WNCK_CFLAGS)
AM_LDFLAGS = $(WNCK_LIBS) $(NOTIFY_LIBS) $(GTK_LIBS) $(GDK_LIBS) $(GIO_UNIX_LIBS) $(CAIRO_LIBS) $(XINERAMA_LIBS) $(XRANDR_LIBS) $(XSCRNSAVER_LIBS) $(XEXT_LIBS) $(X11_LIBS)

bin_PROGRAMS = \
	xde-av-applet \
	xde-bt-applet \
	xde-dh-applet \
	xde-hal-applet \
	xde-id-applet \
	xde-pk-applet \
	xde-sd-applet \
	xde-wl-applet

EXTRA_PROGRAMS = \
	xde-au-applet \
	xde-bl-applet \
	xde-cpu-applet \
	xde-lm-applet \
	xde-up-applet

if HAVE_PULSE
bin_PROGRAMS += xde-au-applet
endif

if HAVE_XRANDR
bin_PROGRAMS += xde-bl-applet
else
if HAVE_GUDEV
bin_PROGRAMS += xde-bl-applet
endif
endif

if HAVE_CPUPOWER
bin_PROGRAMS += xde-cpu-applet
endif

if HAVE_LM_SENSORS
bin_PROGRAMS += xde-lm-applet
endif

if HAVE_UPOWER_GLIB
bin_PROGRAMS += xde-up-applet
endif

xde_au_applet_CPPFLAGS = -DNAME=\"xde-au-applet\"
xde_au_applet_SOURCES = xde-au-applet.c
xde_au_applet_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS) $(SN_CFLAGS) $(SOUND_CFLAGS)
xde_au_applet_LDFLAGS = $(SOUND_LIBS) $(SN_LIBS) $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

xde_av_applet_CPPFLAGS = -DNAME=\"xde-av-applet\"
xde_av_applet_SOURCES = xde-av-applet.c
xde_av_applet_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS) $(SN_CFLAGS) $(SOUND_CFLAGS)
xde_av_applet_LDFLAGS = $(SOUND_LIBS) $(SN_LIBS) $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

xde_bl_applet_CPPFLAGS = -DNAME=\"xde-bl-applet\"
xde_bl_applet_SOURCES = xde-bl-applet.c
xde_bl_applet_CFLAGS = $(AM_CFLAGS) $(XF86VM_LIBS) $(ICE_CFLAGS) $(SMLIB_CFLAGS) $(SN_CFLAGS) $(SOUND_CFLAGS) $(GUDEV_CFLAGS)
xde_bl_applet_LDFLAGS = $(SOUND_LIBS) $(GUDEV_LIBS) $(SN_LIBS) $(SMLIB_LIBS) $(ICE_LIBS) $(XF86VM_LIBS) $(AM_LDFLAGS)

xde_bt_applet_CPPFLAGS = -DNAME=\"xde-bt-applet\"
xde_bt_applet_SOURCES = xde-bt-applet.c
xde_bt_applet_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS) $(SN_CFLAGS) $(SOUND_CFLAGS)
xde_bt_applet_LDFLAGS = $(SOUND_LIBS) $(SN_LIBS) $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

xde_cpu_applet_CPPFLAGS = -DNAME=\"xde-cpu-applet\"
xde_cpu_applet_SOURCES = xde-cpu-applet.c
xde_cpu_applet_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS) $(SN_CFLAGS) $(SOUND_CFLAGS) $(CPU_CFLAGS)
xde_cpu_applet_LDFLAGS = $(CPU_LIBS) $(SOUND_LIBS) $(SN_LIBS) $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

xde_dh_applet_CPPFLAGS = -DNAME=\"xde-dh-applet\"
xde_dh_applet_SOURCES = xde-dh-applet.c
xde_dh_applet_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS) $(SN_CFLAGS) $(SOUND_CFLAGS)
xde_dh_applet_LDFLAGS = $(SOUND_LIBS) $(SN_LIBS) $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

xde_hal_applet_CPPFLAGS = -DNAME=\"xde-hal-applet\"
xde_hal_applet_SOURCES = xde-hal-applet.c
xde_hal_applet_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS) $(SN_CFLAGS) $(SOUND_CFLAGS)
xde_hal_applet_LDFLAGS = $(SOUND_LIBS) $(SN_LIBS) $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

xde_id_applet_CPPFLAGS = -DNAME=\"xde-id-applet\"
xde_id_applet_SOURCES = xde-id-applet.c
xde_id_applet_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS) $(SN_CFLAGS) $(SOUND_CFLAGS)
xde_id_applet_LDFLAGS = $(SOUND_LIBS) $(SN_LIBS) $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

xde_lm_applet_CPPFLAGS = -DNAME=\"xde-lm-applet\"
xde_lm_applet_SOURCES = xde-lm-applet.c
xde_lm_applet_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS) $(SN_CFLAGS) $(SOUND_CFLAGS)
xde_lm_applet_LDFLAGS = -lsensors $(SOUND_LIBS) $(SN_LIBS) $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

xde_pk_applet_CPPFLAGS = -DNAME=\"xde-pk-applet\"
xde_pk_applet_SOURCES = xde-pk-applet.c
xde_pk_applet_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS) $(SN_CFLAGS) $(SOUND_CFLAGS) $(PKGLIB_CFLAGS)
xde_pk_applet_LDFLAGS = $(PKGLIB_LIBS) $(SOUND_LIBS) $(SN_LIBS) $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

xde_sd_applet_CPPFLAGS = -DNAME=\"xde-sd-applet\"
xde_sd_applet_SOURCES = xde-sd-applet.c
xde_sd_applet_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS) $(SN_CFLAGS) $(SOUND_CFLAGS)
xde_sd_applet_LDFLAGS = $(SOUND_LIBS) $(SN_LIBS) $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

xde_up_applet_CPPFLAGS = -DNAME=\"xde-up-applet\"
xde_up_applet_SOURCES = xde-up-applet.c
xde_up_applet_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS) $(SN_CFLAGS) $(SOUND_CFLAGS) $(UPOWER_CFLAGS)
xde_up_applet_LDFLAGS = $(UPOWER_LIBS) $(SOUND_LIBS) $(SN_LIBS) $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

xde_wl_applet_CPPFLAGS = -DNAME=\"xde-wl-applet\"
xde_wl_applet_SOURCES = xde-wl-applet.c
xde_wl_applet_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS) $(SN_CFLAGS) $(SOUND_CFLAGS)
xde_wl_applet_LDFLAGS = $(SOUND_LIBS) $(SN_LIBS) $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

