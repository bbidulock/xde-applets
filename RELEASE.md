[xde-applets -- release notes.  2019-12-21]: #

Preliminary Release 0.7
=======================

This is the seventh release of the xde-applets package.  This package
provides a number of "C"-language applets for the system tray that
provide various functions from package management to wireless
connections.  Some of these applets were originally written in perl(1)
as separate applications.  They have now been converted to "C" for speed
and to provide access to libraries not available from `perl(1)`.

This release is a preliminary release that removes unneeded
dependencies on libxss and libxxf86misc.

Included in the release is an autoconf tarball for building the package
from source.  See the [NEWS](NEWS) and [TODO](TODO) file in the release
for more information.  Please report problems to the issues list on
[GitHub](https://github.com/bbidulock/xde-applets/issues).


[ vim: set ft=markdown sw=4 tw=72 nocin nosi fo+=tcqlorn spell: ]: #
