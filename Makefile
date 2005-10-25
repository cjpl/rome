#####################################################################
#
#  Name:         Makefile
#  Created by:   Ryu Sawada
#
#  $Id:$
#
#####################################################################
INCLUDE = -g -Iinclude -I$(ROMESYS)/include/ -I$(ROMESYS)/builder/include/ -Ibuilder/include/ $(shell root-config --cflags)
LIBRARY = $(shell root-config --libs)
SRC = BuilderConfig.cpp BuilderMonitor.cpp BuilderTab.cpp \
      BuilderWindow.cpp Builder.cpp BuilderNetFolder.cpp main.cpp


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


TARGET =
ifndef ROMESYS	
  TARGET += warning_rome
endif
TARGET += bin/argusbuilder
ifndef ARGUSSYS
  TARGET += warning_argus
endif

all: $(TARGET)

bin/argusbuilder: $(addprefix builder/src/, $(SRC)) $(ROMESYS)/builder/src/ROMEBuilder.cpp $(ROMESYS)/builder/include/ROMEBuilder.h $(ROMESYS)/src/ROMEXML.cpp $(ROMESYS)/src/ROMEString.cpp $(ROMESYS)/src/ROMEStrArray.cpp $(ROMESYS)/src/mxml.c $(ROMESYS)/src/strlcpy.c bin
	@echo 'compiling argusbuilder...'
	@g++ $(CFLAGS) -g -o $@ $(addprefix builder/src/, $(SRC)) $(ROMESYS)/src/ROMEXML.cpp \
	$(ROMESYS)/src/ROMEString.cpp  $(ROMESYS)/src/ROMEStrArray.cpp  $(ROMESYS)/builder/src/ROMEBuilder.cpp $(ROMESYS)/src/mxml.c \
	$(ROMESYS)/src/strlcpy.c $(INCLUDE) $(LIBRARY) \
	|| (echo 'Sorry, compilation was failed. It might be fixed by updating ROME.' ; exit -1;)

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

