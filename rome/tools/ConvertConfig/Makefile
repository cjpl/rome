#####################################################################
#
#  Name:         Makefile
#  Created by:   Matthias Schneebeli
#
# $Id: Makefile 1521 2006-11-08 23:07:05Z sawada $
#
#####################################################################

INCLUDE := -I$(ROMESYS)/include

OBJECTS := main.o \
           mxml.o \
           strlcpy.o

all: convertConfig

convertConfig: $(OBJECTS)
	g++ -g $(INCLUDE) $(OBJECTS) -o convertConfig 
	
main.o: main.cpp
	g++ -g $(INCLUDE) -c main.cpp

mxml.o: $(ROMESYS)/src/mxml.c
	gcc -g $(INCLUDE) -c $(ROMESYS)/src/mxml.c

strlcpy.o: $(ROMESYS)/src/strlcpy.c
	gcc -g $(INCLUDE) -c $(ROMESYS)/src/strlcpy.c

