#####################################################################
#
#  Name:         Makefile
#  Created by:   Matthias Schneebeli
#
# $Id:$
#
#####################################################################
#
INCLUDE :=  -I. -I$(ROMESYS)/include/ -I$(ROMESYS)/builder/include/ $(shell $(ROOTSYS)/bin/root-config --cflags)
LIBRARY := $(shell $(ROOTSYS)/bin/root-config --libs)


OSTYPE = $(shell uname |  tr '[A-Z]' '[a-z]')

ifeq ($(OSTYPE),osf1)
LIBRARY += -lc -lbsd
endif

ifeq ($(OSTYPE),freebsd)
LIBRARY += -lbsd -lcompat
endif

ifeq ($(OSTYPE),darwin)
FINK_DIR := $(shell which fink 2>&1 | sed -ne "s/\/bin\/fink//p")
INCLUDE += -DHAVE_STRLCPY $(shell [ -d $(FINK_DIR)/include ] && echo -I$(FINK_DIR)/include)
LIBRARY += -multiply_defined suppress $(shell [ -d $(FINK_DIR)/lib ] && echo -L$(FINK_DIR)/lib) -lpthread
endif

ifeq ($(OSTYPE),linux)
LIBRARY += -lpthread
endif

ifeq ($(OSTYPE),soralis)
LIBRARY += -lsocket -lnsl
endif

objects :=  obj/strlcpy.o obj/mxml.o obj/ROMEString.o obj/ROMEXML.o obj/ROMEBuilder.o

all: obj $(ROMESYS)/bin/romebuilder.exe

obj:
	@if [ ! -d  obj ] ; then \
		echo "Making directory obj" ; \
		mkdir obj; \
	fi;
$(ROMESYS)/bin/romebuilder.exe: builder/src/main.cpp $(objects)
	g++ $(CFLAGS) -g  $(INCLUDE) -o $@ $< $(objects) $(LIBRARY)

obj/ROMEBuilder.o: builder/src/ROMEBuilder.cpp builder/include/ROMEBuilder.h
	g++ $(CFLAGS) -g  $(INCLUDE) -c -o $@ $<

obj/ROMEXML.o: src/ROMEXML.cpp include/ROMEXML.h
	g++ $(CFLAGS) -g  $(INCLUDE) -c -o $@ $<

obj/ROMEString.o: src/ROMEString.cpp include/ROMEString.h
	g++ $(CFLAGS) -g  $(INCLUDE) -c -o $@ $<

obj/mxml.o: src/mxml.c include/mxml.h
	g++ $(CFLAGS) -g  $(INCLUDE) -c -o $@ $<

obj/strlcpy.o: src/strlcpy.c include/strlcpy.h
	g++ $(CFLAGS) -g  $(INCLUDE) -c -o $@ $<


clean:
	rm -f $(objects)
