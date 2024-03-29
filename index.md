---
layout: default
---
[xde-applets -- read me first file.  2022-01-29]: #

xde-applets
===============

Package `xde-applets-0.12` was released under GPLv3 license
2022-01-29.

This package provides a number of "C"-language applets for the system
tray that provide various functions from package management to wireless
connections.  Some of these applets were originally written in perl(1)
as separate applications.  They have now been converted to "C" for speed
and to provide access to libraries not available from `perl(1)`.

For screenshots and examples, see [SAMPLES](SAMPLES.html).

The source for `xde-applets` is hosted on [GitHub][1].


Release
-------

This is the `xde-applets-0.12` package, released 2022-01-29.
This release, and the latest version, can be obtained from [GitHub][1],
using a command such as:

    $> git clone https://github.com/bbidulock/xde-applets.git

Please see the [RELEASE][3] and [NEWS][4] files for release notes and
history of user visible changes for the current version, and the
[ChangeLog][5] file for a more detailed history of implementation
changes.  The [TODO][6] file lists features not yet implemented and
other outstanding items.

Please see the [INSTALL][8] file for installation instructions.

When working from `git(1)`, please use this file.  An abbreviated
installation procedure that works for most applications appears below.

This release is published under GPLv3.  Please see the license in the
file [COPYING][10].


Quick Start
-----------

The quickest and easiest way to get `xde-applets` up and
running is to run the following commands:

    $> git clone https://github.com/bbidulock/xde-applets.git
    $> cd xde-applets
    $> ./autogen.sh
    $> ./configure
    $> make
    $> make DESTDIR="$pkgdir" install

This will configure, compile and install `xde-applets` the
quickest.  For those who like to spend the extra 15 seconds reading
`./configure --help`, some compile time options can be turned on and off
before the build.

For general information on GNU's `./configure`, see the file
[INSTALL][8].


Running
-------

Read the manual page after installation:

    $> man xde-applets


Issues
------

Report issues on GitHub [here][2].



[1]: https://github.com/bbidulock/xde-applets
[2]: https://github.com/bbidulock/xde-applets/issues
[3]: https://github.com/bbidulock/xde-applets/blob/0.12/RELEASE
[4]: https://github.com/bbidulock/xde-applets/blob/0.12/NEWS
[5]: https://github.com/bbidulock/xde-applets/blob/0.12/ChangeLog
[6]: https://github.com/bbidulock/xde-applets/blob/0.12/TODO
[7]: https://github.com/bbidulock/xde-applets/blob/0.12/COMPLIANCE
[8]: https://github.com/bbidulock/xde-applets/blob/0.12/INSTALL
[9]: https://github.com/bbidulock/xde-applets/blob/0.12/LICENSE
[10]: https://github.com/bbidulock/xde-applets/blob/0.12/COPYING

[ vim: set ft=markdown sw=4 tw=72 nocin nosi fo+=tcqlorn spell: ]: #
