#####################################################################
#
#  Name:         Makefile
#  Created by:   Ryu Sawada
#
#  $Log$
#  Revision 1.11  2005/07/04 14:39:38  sawada
#  error message when compilation is failed.
#
#  Revision 1.10  2005/05/30 18:22:15  sawada
#  small change for Macintosh
#
#  Revision 1.9  2005/05/26 14:26:54  sawada
#  Lots of changes.
#  Made ArgusBuilder an inheriting class of ROMEBuilder.
#  Remove ROMEFolder and added NetFolers.
#  Added ArgusWindow class.
#  and so on.
#
#  Revision 1.8  2005/05/16 16:46:57  sawada
#  with gcc4, enumeration in class can not be constant in switch.
#
#  Revision 1.7  2005/05/08 00:28:53  sawada
#  fixed mismathes of [Set,Append]Formatted in builder.
#  added readme of examples.
#
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

bin/argusbuilder: $(addprefix builder/src/, $(SRC)) $(ROMESYS)/builder/src/ROMEBuilder.cpp $(ROMESYS)/builder/include/ROMEBuilder.h $(ROMESYS)/src/ROMEXML.cpp $(ROMESYS)/src/ROMEString.cpp $(ROMESYS)/src/mxml.c $(ROMESYS)/src/strlcpy.c bin
	@echo 'compiling argusbuilder...'
	@g++ $(CFLAGS) -g -o $@ $(addprefix builder/src/, $(SRC)) $(ROMESYS)/src/ROMEXML.cpp \
	$(ROMESYS)/src/ROMEString.cpp  $(ROMESYS)/builder/src/ROMEBuilder.cpp $(ROMESYS)/src/mxml.c \
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

