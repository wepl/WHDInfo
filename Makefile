#
# Makefile for WHDInfo
# on Amiga use gmake/gcc and call 'make AMIGA=1'
# on MacOSX use XCode and call 'make MACOSX=1'
# on AIX use xlc and call 'gmake AIX=1' (requires GNU make, see Toolbox)
#
# $@ target
# $< first dependency
# $^ all dependencies

RM=rm -f

CFLAGS = -Wall -O2

ifdef AMIGA
CC = gcc
CFLAGS = -Wall -noixemul -O2
LFLAGS = -noixemul -s
endif

ifdef MACOSX
#ARCH=-arch i386 -arch ppc
#ARCH=-arch i386 -arch x86_64	# source is not yet 64-bit ready!
ARCH=-arch i386
CFLAGS = -Wall -O2 $(ARCH)
LFLAGS = $(ARCH)
endif

ifdef AIX
CFLAGS = -O2 -qcpluscmt
LFLAGS =
endif

SRCS = WHDInfo.c
OBJS = $(SRCS:.c=.o)

ifdef AMIGA

/C/WHDInfo : $(OBJS)
	$(CC) $(LFLAGS) -o $@ $^

else

WHDInfo : $(OBJS)
	$(CC) $(LFLAGS) -o $@ $^

endif

clean :
	$(RM) *.bak *.o *.lnk *.tmp WHDInfo

distclean : clean
	$(RM) C:WHDInfo

depend:
	makedepend -f Makefile $(OBJS)

#DEPENDENCIES

WHDInfo.o: WHDInfo.c whdload.h exectypes.h

