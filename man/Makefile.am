
xde_man1_files = \
	av-applet.man \
	bt-applet.man \
	dh-applet.man \
	hal-applet.man \
	pk-applet.man \
	sd-applet.man \
	wl-applet.man \
	xde-applets.man

nodist_man1_MANS = $(xde_man1_files)
CLEANFILES = $(xde_man1_files)

EXTRA_DIST = \
	av-applet.pod \
	bt-applet.pod \
	dh-applet.pod \
	hal-applet.pod \
	pk-applet.pod \
	sd-applet.pod \
	wl-applet.pod \
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

#$(xde_man5_files): %.man: %.pod
#	$(AM_V_GEN)pod2man --center="Standards, Environments and Macros" \
#		--date="$(DATE)" \
#		--errors="none" \
#		--fixed="CW" \
#		--fixedbold="CB" \
#		--fixeditalic="CI" \
#		--fixedbolditalic="CX" \
#		--nourls \
#		--release="$(PACKAGE) $(VERSION)" \
#		--section="5" \
#		$< >$@

