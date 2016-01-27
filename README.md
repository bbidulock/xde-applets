
## xde-applets

Package xde-applets- was released under GPL license .

This package provides a number of "C"-language applets for the system
tray that provide various functions from package management to wireless
connections.  Some of these applets were originally written in perl(1)
as separate applications.  They have now been converted to "C" for speed
and to provide access to libraries not available from perl(1).


### Release

This is the `xde-applets-` package, released .  This release,
and the latest version, can be obtained from the GitHub repository at
http://github.com/bbidulock/xde-applets, using a command such as:

```bash
git clone http://github.com/bbidulock/xde-applets.git
```

Please see the [NEWS](NEWS) file for release notes and history of user visible
changes for the current version, and the [ChangeLog](ChangeLog) file for a more
detailed history of implementation changes.  The [TODO](TODO) file lists
features not yet implemented and other outstanding items.

Please see the [INSTALL](INSTALL) file for installation instructions.

When working from `git(1)', please see the [README-git](README-git) file.  An
abbreviated installation procedure that works for most applications
appears below.

This release is published under the GPL license that can be found in
the file [COPYING](COPYING).

### Quick Start:

The quickest and easiest way to get xde-applets up and running is to run
the following commands:

```bash
git clone http://github.com/bbidulock/xde-applets.git xde-applets
cd xde-applets
./autogen.sh
./configure --prefix=/usr
make V=0
make DESTDIR="$pkgdir" install
```

This will configure, compile and install xde-applets the quickest.  For
those who would like to spend the extra 15 seconds reading `./configure
--help`, some compile time options can be turned on and off before the
build.

For general information on GNU's `./configure`, see the file [INSTALL](INSTALL).

### Running xde-applets

Read the manual pages after installation:

    man xde-applets

### Issues

Report issues to http://github.com/bbidulock/xde-applets/issues.

