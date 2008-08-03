#
# Makefile for WHDInfo
# use gmake/gcc and call 'make AMIGA=yes'
#
# $Id: Makefile 1.2 2005/07/13 16:39:24 wepl Exp wepl $
#
# 01.08.08 created
#
# $@ target
# $< first dependency
# $^ all dependencies

RM=rm -f

ifdef AMIGA

CFLAGS = -Wall -noixemul -O2
LFLAGS = -noixemul -s

else

CFLAGS = -Wall -O2
LFLAGS = -s

endif

SRCS = WHDInfo.c endian.c
OBJS = $(SRCS:.c=.o)

ifdef AMIGA

/C/WHDInfo : $(OBJS)
	$(CC) $(LFLAGS) -o $@ $^

else

WHDInfo : $(OBJS)
	$(CC) $(LFLAGS) -o $@ $^

endif

clean :
	$(RM) *.bak *.o *.lnk

distclean : clean
	$(RM) C:WHDInfo

depend:
	makedepend -f Makefile $(OBJS)

#DEPENDENCIES

WHDInfo.o: WHDInfo.c WHDInfo.h exectypes.h endian.h

endian.o: endian.c endian.h exectypes.h

