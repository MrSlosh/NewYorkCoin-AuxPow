
Debian
====================
This directory contains files used to package newyorkcoind/newyorkcoin-qt
for Debian-based Linux systems. If you compile newyorkcoind/newyorkcoin-qt yourself, there are some useful files here.

## newyorkcoin: URI support ##


newyorkcoin-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install newyorkcoin-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your newyorkcoin-qt binary to `/usr/bin`
and the `../../share/pixmaps/bitcoin128.png` to `/usr/share/pixmaps`

newyorkcoin-qt.protocol (KDE)

