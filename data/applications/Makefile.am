dist_applications_DATA = \
	xde-av-applet.desktop \
	xde-bt-applet.desktop \
	xde-dh-applet.desktop \
	xde-hal-applet.desktop \
	xde-id-applet.desktop \
	xde-pk-applet.desktop \
	xde-sd-applet.desktop \
	xde-wl-applet.desktop

EXTRA_DIST = \
	xde-au-applet.desktop \
	xde-bl-applet.desktop \
	xde-cpu-applet.desktop \
	xde-lm-applet.desktop \
	xde-up-applet.desktop

if HAVE_PULSE
dist_applications_DATA += xde-au-applet.desktop
endif

if HAVE_XRANDR
dist_applications_DATA += xde-bl-applet.desktop
else
if HAVE_GUDEV
dist_applications_DATA += xde-bl-applet.desktop
endif
endif

if HAVE_CPUPOWER
dist_applications_DATA += xde-cpu-applet.desktop
endif

if HAVE_LM_SENSORS
dist_applications_DATA += xde-lm-applet.desktop
endif

if HAVE_UPOWER_GLIB
dist_applications_DATA += xde-up-applet.desktop
endif




