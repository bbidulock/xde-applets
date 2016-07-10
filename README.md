[xde-applets -- read me first file.  2016-07-09]: #

xde-applets
===========

Package `xde-applets-0.4` was released under GPL license 2016-07-09.

This package provides a number of "C"-language applets for the system
tray that provide various functions from package management to wireless
connections.  Some of these applets were originally written in perl(1)
as separate applications.  They have now been converted to "C" for speed
and to provide access to libraries not available from `perl(1)`.


Release
-------

This is the `xde-applets-0.4` package, released 2016-07-09.  This release,
and the latest version, can be obtained from the [GitHub repository][1],
using a command such as:

    $> git clone https://github.com/bbidulock/xde-applets.git

Please see the [NEWS][2] file for release notes and history of user visible
changes for the current version, and the [ChangeLog][3] file for a more
detailed history of implementation changes.  The [TODO][4] file lists
features not yet implemented and other outstanding items.

Please see the [INSTALL][5] file for installation instructions.

When working from `git(1)`, please use this file.  An abbreviated
installation procedure that works for most applications appears below.

This release is published under the GPL license that can be found in
the file [COPYING][6].


Quick Start
-----------

The quickest and easiest way to get `xde-applets` up and running is to
run the following commands:

    $> git clone https://github.com/bbidulock/xde-applets.git xde-applets
    $> cd xde-applets
    $> ./autogen.sh
    $> ./configure --prefix=/usr
    $> make V=0
    $> make DESTDIR="$pkgdir" install

This will configure, compile and install `xde-applets` the quickest.  For
those who would like to spend the extra 15 seconds reading `./configure
--help`, some compile time options can be turned on and off before the
build.

For general information on GNU's `./configure`, see the [INSTALL][5] file.


Running
-------

Read the manual pages after installation:

    man xde-applets


Issues
------

Report issues at GitHub [here][7].



[1]: https://github.com/bbidulock/xde-applets
[2]: NEWS
[3]: ChangeLog
[4]: TODO
[5]: INSTALL
[6]: COPYING
[7]: https://github.com/bbidulock/xde-applets/issues

[ vim: set ft=markdown sw=4 tw=80 nocin nosi fo+=tcqlorn spell: ]: #
