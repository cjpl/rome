#####################################################################
#
#  Name:         Makefile
#  Created by:   Ryu Sawada
#
#  $Log$
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
INCLUDE := -I$(ROMESYS)/include/ -Ibuilder/include/ $(shell root-config --cflags) $(shell xml2-config --cflags)
LIBRARY := -g $(shell root-config --libs) $(shell xml2-config --libs) -lpthread
SRC = BuilderConfig.cpp BuilderMonitor.cpp BuilderTab.cpp BuilderFolder.cpp \
      BuilderSteering.cpp BuilderWindow.cpp Builder.cpp

## for Macintosh
ifeq ($(shell uname),Darwin)
FINK_DIR := $(shell which fink 2>&1 | sed -ne "s/\/bin\/fink//p")
Include += -DHAVE_STRLCPY $(shell [ -d $(FINK_DIR)/include ] && echo -I$(FINK_DIR)/include)
LIBRARY += -multiply_defined suppress $(shell [ -d $(FINK_DIR)/lib ] && echo -L$(FINK_DIR)/lib)
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

bin/argusbuilder: $(addprefix builder/src/, $(SRC)) $(ROMESYS)/src/ROMEXML.cpp $(ROMESYS)/src/ROMEString.cpp bin
	g++ $(CFLAGS) -o $@ $(addprefix builder/src/, $(SRC)) $(ROMESYS)/src/ROMEXML.cpp \
	$(ROMESYS)/src/ROMEString.cpp $(INCLUDE) $(LIBRARY)

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

