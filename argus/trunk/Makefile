#####################################################################
#
#  Name:         Makefile
#  Created by:   Ryu Sawada
#
#  $Log$
#  Revision 1.6  2005/05/06 18:40:12  sawada
#  added ArgusVersion.h
#
#  Revision 1.5  2005/05/06 18:37:05  sawada
#  added ArgusVersion.h
#
#  Revision 1.4  2005/03/26 16:53:42  sawada
#  libxml replaced by mxml.
#
#  Revision 1.3  2005/02/21 23:07:50  sawada
#  several UNIX support
#
#  Revision 1.2  2005/01/30 20:39:39  sawada
#  * Makefile of builder
#  * Tab enable/disable
#  * Bug fix.(fNetFolder, ConnectServer)
#
#  Revision 1.1.1.1  2005/01/29 22:45:07  sawada
#  Advanced Root based GUi monitoring System
#
#
#####################################################################
#
INCLUDE = -g -Iinclude -I$(ROMESYS)/include/ -Ibuilder/include/ $(shell root-config --cflags)
LIBRARY = -g $(shell root-config --libs)
SRC = BuilderConfig.cpp BuilderMonitor.cpp BuilderTab.cpp BuilderFolder.cpp \
      BuilderSteering.cpp BuilderWindow.cpp Builder.cpp


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


TARGET =
ifndef ROMESYS	
  TARGET += warning_rome
endif
TARGET += bin/argusbuilder
ifndef ARGUSSYS
  TARGET += warning_argus
endif

all: $(TARGET)

bin/argusbuilder: $(addprefix builder/src/, $(SRC)) $(ROMESYS)/src/ROMEXML.cpp $(ROMESYS)/src/ROMEString.cpp $(ROMESYS)/src/mxml.c bin
	g++ $(CFLAGS) -o $@ $(addprefix builder/src/, $(SRC)) $(ROMESYS)/src/ROMEXML.cpp \
	$(ROMESYS)/src/ROMEString.cpp $(ROMESYS)/src/mxml.c $(INCLUDE) $(LIBRARY)

bin:
	@if [ ! -d  bin ] ; then \
		echo "Making directory bin" ; \
		mkdir bin; \
	fi;

warning_argus::
	@echo 'Please set environment variable ARGUSSYS'

warning_rome::
	@echo 'Please set environment variable ROMESYS'
	@exit 1

