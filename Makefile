#####################################################################
#
#  Name:         Makefile
#  Created by:   Ryu Sawada
#
#  $Id$
#
#####################################################################
INCLUDE = -g -D__ARGUS__ -Iinclude -I$(ROMESYS)/include/ -I$(ROMESYS)/builder/include/ -Ibuilder/include/ $(shell root-config --cflags)
LIBRARY = $(shell root-config --libs)
objects = obj/strlcpy.o obj/mxml.o obj/ROMEString.o obj/ROMEStrArray.o obj/ROMEXML.o \
          obj/ROMEBuilder.o obj/Builder.o obj/BuilderConfig.o \
          obj/BuilderMonitor.o obj/BuilderTab.o obj/BuilderWindow.o \
          obj/BuilderNetFolder.o

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

all: obj $(TARGET)

bin/argusbuilder: builder/src/main.cpp $(objects) bin
	@echo 'linking argusbuilder...'
	@g++ $(CFLAGS) $(INCLUDE) -g -o $@ $< $(objects) $(LIBRARY) \
	|| (echo 'Sorry, compilation was failed. It might be fixed by updating ROME.' ; exit -1;)

obj/strlcpy.o: $(ROMESYS)/src/strlcpy.c $(ROMESYS)/include/strlcpy.h
	g++ $(CFLAGS)  $(INCLUDE) -c -g -o $@ $<

obj/mxml.o: $(ROMESYS)/src/mxml.c  $(ROMESYS)/include/mxml.h
	g++ $(CFLAGS)  $(INCLUDE) -c -g -o $@ $<

obj/ROMEString.o: $(ROMESYS)/src/ROMEString.cpp  $(ROMESYS)/include/ROMEString.h
	g++ $(CFLAGS)  $(INCLUDE) -c -g -o $@ $<

obj/ROMEStrArray.o: $(ROMESYS)/src/ROMEStrArray.cpp  $(ROMESYS)/include/ROMEStrArray.h
	g++ $(CFLAGS)  $(INCLUDE) -c -g -o $@ $<

obj/ROMEXML.o: $(ROMESYS)/src/ROMEXML.cpp  $(ROMESYS)/include/ROMEXML.h
	g++ $(CFLAGS)  $(INCLUDE) -c -g -o $@ $<

obj/ROMEBuilder.o: $(ROMESYS)/builder/src/ROMEBuilder.cpp $(ROMESYS)/builder/include/ROMEBuilder.h
	g++ $(CFLAGS)  $(INCLUDE) -c -g -o $@ $<

obj/Builder.o: builder/src/Builder.cpp
	@echo 'compiling obj/Builder.o...'
	@g++ $(CFLAGS)  $(INCLUDE) -c -g -o $@ $< \
	|| (echo 'Sorry, compilation was failed. It might be fixed by updating ROME.' ; exit -1;)

obj/BuilderConfig.o: builder/src/BuilderConfig.cpp
	@echo 'compiling obj/BuilderConfig.o...'
	@g++ $(CFLAGS)  $(INCLUDE) -c -g -o $@ $< \
	|| (echo 'Sorry, compilation was failed. It might be fixed by updating ROME.' ; exit -1;)

obj/BuilderMonitor.o: builder/src/BuilderMonitor.cpp
	@echo 'compiling obj/BuilderMonitor.o...'
	@g++ $(CFLAGS)  $(INCLUDE) -c -g -o $@ $< \
	|| (echo 'Sorry, compilation was failed. It might be fixed by updating ROME.' ; exit -1;)

obj/BuilderTab.o: builder/src/BuilderTab.cpp
	@echo 'compiling obj/BuilderTab.o...'
	@g++ $(CFLAGS)  $(INCLUDE) -c -g -o $@ $< \
	|| (echo 'Sorry, compilation was failed. It might be fixed by updating ROME.' ; exit -1;)

obj/BuilderWindow.o: builder/src/BuilderWindow.cpp
	@echo 'compiling obj/BuilderWindow.o...'
	@g++ $(CFLAGS)  $(INCLUDE) -c -g -o $@ $< \
	|| (echo 'Sorry, compilation was failed. It might be fixed by updating ROME.' ; exit -1;)

obj/BuilderNetFolder.o: builder/src/BuilderNetFolder.cpp
	@echo 'compiling obj/BuilderNetFolder.o...'
	@g++ $(CFLAGS)  $(INCLUDE) -c -g -o $@ $< \
	|| (echo 'Sorry, compilation was failed. It might be fixed by updating ROME.' ; exit -1;)

bin:
	@if [ ! -d  bin ] ; then \
		echo "Making directory bin" ; \
		mkdir bin; \
	fi;

obj:
	@if [ ! -d  obj ] ; then \
		echo "Making directory obj" ; \
		mkdir obj; \
	fi;

warning_argus::
	@echo 'Please set environment variable ARGUSSYS'

warning_rome::
	@echo 'Please set environment variable ROMESYS'
	@exit 1

clean:
	rm -f obj/*.o
