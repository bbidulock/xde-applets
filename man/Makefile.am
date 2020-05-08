
xde_man1_files = \
	xde-au-applet.man \
	xde-av-applet.man \
	xde-bl-applet.man \
	xde-bt-applet.man \
	xde-dh-applet.man \
	xde-hal-applet.man \
	xde-id-applet.man \
	xde-pk-applet.man \
	xde-sd-applet.man \
	xde-wl-applet.man

xde_man5_files = \
	xde-applets.man

if HAVE_CPUPOWER
xde_man1_files += xde-cpu-applet.man
endif

if HAVE_LM_SENSORS
xde_man1_files += xde-lm-applet.man
endif

if HAVE_UPOWER_GLIB
xde_man1_files += xde-up-applet.man
endif

nodist_man1_MANS = $(xde_man1_files)
nodist_man5_MANS = $(xde_man5_files)
CLEANFILES = $(xde_man1_files) $(xde_man5_files)

EXTRA_DIST = \
	xde-au-applet.pod \
	xde-av-applet.pod \
	xde-bl-applet.pod \
	xde-bt-applet.pod \
	xde-cpu-applet.pod \
	xde-dh-applet.pod \
	xde-hal-applet.pod \
	xde-id-applet.pod \
	xde-lm-applet.pod \
	xde-pk-applet.pod \
	xde-sd-applet.pod \
	xde-up-applet.pod \
	xde-wl-applet.pod \
	xde-applets.pod

$(xde_man1_files): %.man: %.pod
	$(AM_V_GEN)pod2man --center="User Commands" \
		--date="$(DATE)" \
		--errors="none" \
		--fixed="CW" \
		--fixedbold="CB" \
		--fixeditalic="CI" \
		--fixedbolditalic="CX" \
		--nourls \
		--release="$(PACKAGE) $(VERSION)" \
		--section="1" \
		$< >$@

$(xde_man5_files): %.man: %.pod
	$(AM_V_GEN)pod2man --center="Standards, Environments and Macros" \
		--date="$(DATE)" \
		--errors="none" \
		--fixed="CW" \
		--fixedbold="CB" \
		--fixeditalic="CI" \
		--fixedbolditalic="CX" \
		--nourls \
		--release="$(PACKAGE) $(VERSION)" \
		--section="5" \
		$< >$@

