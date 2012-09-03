#
#	File:		Makefile
#
#	Contains:	xxx put contents here xxx
#
#	Written by:	David Hankinson
#
#	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.
#
#	Change History (most recent first):
#
#		 <1>	 7/15/94	dwh		first checked in
#
#	To Do:
#

include ../../UNIX/Makefile.config

INCOPTS=-I/usr/include \
	-I../../Comm \
	-I../../SegaOS/Database \
	-I../../SegaOS/Graphics \
	-I../../SegaOS/Interfaces \
	-I../../SegaOS/OSCore \
	-I../../SegaOS/Shell \
	-I../../SegaOS/Misc \
	-I../Misc \
	-I../Server \
	-I../ServerDataBase

OBJS =	Errors.o

all: $(OBJS)

clean::
	$(RM) $(OBJS)
