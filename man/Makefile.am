
xde_man_files = \
	av-applet.man \
	bt-applet.man \
	dh-applet.man \
	hal-applet.man \
	pk-applet.man \
	sd-applet.man \
	wl-applet.man \
	xde-applets.man

nodist_man1_MANS = $(xde_man_files)
CLEANFILES = $(xde_man_files)

EXTRA_DIST = \
	av-applet.pod \
	bt-applet.pod \
	dh-applet.pod \
	hal-applet.pod \
	pk-applet.pod \
	sd-applet.pod \
	wl-applet.pod \
	xde-applets.pod

SUFFIXES = .man .pod

.pod.man:
	pod2man --center="User Commands" \
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


