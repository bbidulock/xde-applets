[xde-applets -- read me first file.  2018-10-13]: #

xde-applets
===============

Package `xde-applets-0.4.54` was released under GPLv3 license 2018-10-13.

This package provides a number of "C"-language applets for the system
tray that provide various functions from package management to wireless
connections.  Some of these applets were originally written in perl(1)
as separate applications.  They have now been converted to "C" for speed
and to provide access to libraries not available from `perl(1)`.


Release
-------

This is the `xde-applets-0.4.54` package, released 2018-10-13.  This
release, and the latest version, can be obtained from [GitHub][1], using
a command such as:

    $> git clone https://github.com/bbidulock/xde-applets.git

Please see the [NEWS][3] file for release notes and history of user
visible changes for the current version, and the [ChangeLog][4] file for
a more detailed history of implementation changes.  The [TODO][5] file
lists features not yet implemented and other outstanding items.

Please see the [INSTALL][7] file for installation instructions.

When working from `git(1)`, please use this file.  An abbreviated
installation procedure that works for most applications appears below.

This release is published under GPLv3.  Please see the license in the
file [COPYING][9].


Quick Start
-----------

The quickest and easiest way to get `xde-applets` up and running is to run
the following commands:

    $> git clone https://github.com/bbidulock/xde-applets.git
    $> cd xde-applets
    $> ./autogen.sh
    $> ./configure
    $> make
    $> make DESTDIR="$pkgdir" install

This will configure, compile and install `xde-applets` the quickest.  For
those who like to spend the extra 15 seconds reading `./configure
--help`, some compile time options can be turned on and off before the
build.

For general information on GNU's `./configure`, see the file
[INSTALL][7].


Running
-------

Read the manual page after installation:

    man xde-applets


Issues
------

Report issues on GitHub [here][2].



[1]: https://github.com/bbidulock/xde-applets
[2]: https://github.com/bbidulock/xde-applets/issues
[3]: https://github.com/bbidulock/xde-applets/blob/master/NEWS
[4]: https://github.com/bbidulock/xde-applets/blob/master/ChangeLog
[5]: https://github.com/bbidulock/xde-applets/blob/master/TODO
[6]: https://github.com/bbidulock/xde-applets/blob/master/COMPLIANCE
[7]: https://github.com/bbidulock/xde-applets/blob/master/INSTALL
[8]: https://github.com/bbidulock/xde-applets/blob/master/LICENSE
[9]: https://github.com/bbidulock/xde-applets/blob/master/COPYING

[ vim: set ft=markdown sw=4 tw=72 nocin nosi fo+=tcqlorn spell: ]: #
