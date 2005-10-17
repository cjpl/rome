#####################################################################
#
#  Name:         Makefile
#  Created by:   Matthias Schneebeli
#
#  $Log$
#  Revision 1.23  2005/10/06 21:46:13  sawada
#  improved Makefile for builder.
#
#  Revision 1.22  2005/09/16 15:44:16  sawada
#  improved Makefile for UNIX.
#
#  Revision 1.21  2005/05/30 18:21:13  sawada
#  small change for Macintosh
#
#  Revision 1.20  2005/05/17 18:45:38  sawada
#  Separated main from ROMEBuilder.
#  Reading function for every database folders.
#
#  Revision 1.19  2005/05/14 16:07:27  sawada
#  Improved performance with changing TString::ResizeIncrement to 4096.
#
#  Revision 1.18  2005/05/13 20:20:54  sawada
#  separated writing function of files.
#  placeholder of user html.
#
#  Revision 1.17  2005/05/11 12:50:01  schneebeli_m
#  added strlcpy
#
#  Revision 1.16  2005/03/23 09:12:21  schneebeli_m
#  libxml replaced by mxml
#
#  Revision 1.15  2005/02/21 23:07:28  sawada
#  not compile when builder is up to data.
#
#  Revision 1.14  2005/02/21 22:47:31  sawada
#  Support for DEC,Ultrix,FreeBSD,Solaris
#
#  Revision 1.13  2004/12/09 08:18:00  schneebeli_m
#  -lg2c removed
#
#  Revision 1.12  2004/12/02 17:46:43  sawada
#  Macintosh port
#
#  Revision 1.11  2004/10/25 08:34:06  schneebeli_m
#  *** empty log message ***
#
#  Revision 1.10  2004/10/21 13:03:51  schneebeli_m
#  *** empty log message ***
#
#  Revision 1.9  2004/10/20 00:18:32  pierre
#  correct Midas support and romebuilder image name
#
#  Revision 1.8  2004/10/19 22:08:27  pierre
#  add MIDAS support
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
