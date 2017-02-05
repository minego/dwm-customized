# dwm version
VERSION = 6.1

# Customize below to fit your system

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

# Xinerama, comment if you don't want it
XINERAMALIBS  = -lXinerama
XINERAMAFLAGS = -DXINERAMA

# includes and libs
INCS = -I${X11INC} -I/usr/include/freetype2
LIBS = -L${X11LIB} -lX11 ${XINERAMALIBS} -lfontconfig -lXft ${FREETYPELIBS} -lX11-xcb -lxcb -lxcb-res

# flags
CPPFLAGS = -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=2 -DVERSION=\"${VERSION}\" ${XINERAMAFLAGS}
#CFLAGS   = -g -std=c99 -pedantic -Wall -O0 ${INCS} ${CPPFLAGS}
#CFLAGS   = -g -ggdb -DDEBUG -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Os ${INCS} ${CPPFLAGS}
CFLAGS   = -g -ggdb -DDEBUG -std=c99 -pedantic -Wall -Wno-deprecated-declarations -O0 ${INCS} ${CPPFLAGS}
LDFLAGS  = -g -ggdb ${LIBS}

# Solaris
#CFLAGS = -fast ${INCS} -DVERSION=\"${VERSION}\"
#LDFLAGS = ${LIBS}

# compiler and linker
CC = gcc

