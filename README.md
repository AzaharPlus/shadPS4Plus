# ShadPs4Plus is a standalone PKG extractor based on code that used to be in ShadPS4.

It has two main parts:

# pkg_extractor exe / AppImage

This is the extractor proper, it is a command line tool.
The first argument is the path to the pkg file to extract.
The second argument is where to extract it.
If no second argument is provided, it will be extracted next to the pkg file.

On Windows, you can drag and drop the pkg file onto the exe and it will extract it at the same location.

# install_pkg sh / bat

This script has to be edited before use.
Set the values for gamesDir and addonsDir to the ones used in ShadPS4 (you can check in settings > paths).

The first argument is the path to the pkg file to extract.

On Windows, you can drag and drop the pkg file onto the bat and it will extract it where it needs to be.
