#
# Makefile for WHDInfo
# on Amiga use gmake/gcc and call 'make AMIGA=1'
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

