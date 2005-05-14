#####################################################################
#
#  Name:         Makefile
#  Created by:   Matthias Schneebeli
#
#  $Log$
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
INCLUDE :=  -I$(ROMESYS)/include/ -I$(ROMESYS)/builder/include/ $(shell root-config --cflags)
LIBRARY := $(shell root-config --libs)


OSTYPE = $(shell uname |  tr '[A-Z]' '[a-z]')

ifeq ($(OSTYPE),osf1)
LIBRARY += -lc -lbsd
endif

ifeq ($(OSTYPE),freebsd)
LIBRARY += -lbsd -lcompat
endif

ifeq ($(OSTYPE),darwin)
FINK_DIR := $(shell which fink 2>&1 | sed -ne "s/\/bin\/fink//p")
Include += -DHAVE_STRLCPY $(shell [ -d $(FINK_DIR)/include ] && echo -I$(FINK_DIR)/include)
LIBRARY += -multiply_defined suppress $(shell [ -d $(FINK_DIR)/lib ] && echo -L$(FINK_DIR)/lib) -lpthread
endif

ifeq ($(OSTYPE),linux)
LIBRARY += -lpthread
endif

ifeq ($(OSTYPE),soralis)
LIBRARY += -lsocket -lnsl
endif

$(ROMESYS)/bin/romebuilder.exe: builder/src/ROMEBuilder.cpp builder/include/ROMEBuilder.h src/ROMEXML.cpp src/mxml.c src/strlcpy.c src/ROMEString.cpp
	g++ $(CFLAGS) -g -o $@ builder/src/ROMEBuilder.cpp src/ROMEXML.cpp src/mxml.c src/strlcpy.c \
	src/ROMEString.cpp $(INCLUDE) $(LIBRARY)

clean:
	rm -f $(ROMESYS)/bin/romebuilder.exe
