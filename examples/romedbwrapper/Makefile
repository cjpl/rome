OSTYPE = $(shell uname |  tr '[A-Z]' '[a-z]')

## Compilers
CXX   ?= g++
CC    ?= gcc
FC    ?= g77

-include ../../Makefile.arch

ifdef ROOTSYS
ROOTCONFIG := $(ROOTSYS)/bin/root-config
ROOTCINT   := $(ROOTSYS)/bin/rootcint
else
ROOTCONFIG := root-config
ROOTCINT   := rootcint
endif

ROOT_VERSION := $(shell $(ROOTCONFIG) --version)
ROOT_MAJOR := $(shell $(ROOTCONFIG) --version 2>&1 | cut -d'.' -f1)
ROOT_MINOR := $(shell $(ROOTCONFIG) --version 2>&1 | cut -d'/' -f1 | cut -d'.' -f2)
ROOT_PATCH := $(shell $(ROOTCONFIG) --version 2>&1 | cut -d'/' -f2)

NEED_TARRAYL64 = no
ifeq ($(ROOT_MAJOR), 5)
ifeq ($(shell expr $(ROOT_MINOR) \< 14), 1)
NEED_TARRAYL64 = yes
endif
endif
ifeq ($(ROOT_MAJOR), 4)
NEED_TARRAYL64 = yes
endif

## Compile and link flags
rootlibs  := $(shell $(ROOTCONFIG) --libs)
rootcflags:= $(shell $(ROOTCONFIG) --cflags)
sqllibs   :=  -lsqlite3
sqlcflags := -DHAVE_SQL -DHAVE_SQLITE3

CFlags    := $(OSCFLAGS) $(rootcflags) $(sqlcflags) -g -Wall -W
FFlags    := -g -Wall -W
Libraries :=
Libraries += $(oslibs) $(sqllibs) $(rootlibs)
LDFLAGS   := $(osldflags)

## Include directories
Includes := -I$(ROMESYS)/include/
ifeq ($(NEED_TARRAYL64), yes)
Includes += -DNEED_TARRAYL64 -I$(ROMESYS)/include/array64
endif

## Objects
objects += ROMESQLite3.o \
           ROMESQL.o \
           ROMESQLDataBase.o \
           ROMETextDataBase.o \
           ROMEDataBaseWrapper.o \
           ROMEStr2DArray.o \
           ROMEStrArray.o \
           ROMEString.o \
           ROMEPrint.o \
           ROMEPath.o \
           Dict.o

DICTHEADERS = $(ROMESYS)/include/ROMEString.h \
              $(ROMESYS)/include/ROMEStrArray.h \
              $(ROMESYS)/include/ROMEStr2DArray.h \
              $(ROMESYS)/include/ROMEPath.h \
              $(ROMESYS)/include/ROMESQL.h \
              $(ROMESYS)/include/ROMESQLite3.h \
              $(ROMESYS)/include/ROMESQLDataBase.h \
              $(ROMESYS)/include/ROMETextDataBase.h \
              $(ROMESYS)/include/ROMEDataBase.h

ifeq ($(OSTYPE), darwin)
CXX_LIBS = -L$(dir $(shell $(CXX) -print-libgcc-file-name)) \
           -lstdc++ -lgcc_eh
endif

##
## Compile and link rules
##
all: test_c test_f

## Link statements
test_c: $(objects) main_c.o
	$(CXX) $(LDFLAGS) -o $@ $(objects) main_c.o $(Libraries)

test_f: $(objects) main_f.o
	$(FC) $(LDFLAGS) -o $@ main_f.o $(objects) $(CXX_LIBS) $(Libraries)

ROME%.o : $(ROMESYS)/src/ROME%.cpp
	$(CXX) -c $(CFlags) $(Includes) -MMD -MP -MF $(@:.o=.d) -MT $@ $< -o $@

main_c.o : main_c.c
	$(CC) -c $(CFlags) $(Includes) -MMD -MP -MF $(@:.o=.d) -MT $@ $< -o $@

main_f.o : main_f.F
	$(FC) -c $(FFlags) $(Includes) $< -o $@

Dict.o : Dict.cpp Dict.h
	$(CXX) -c $(CFlags) $(Includes) -MMD -MP -MF $(@:.o=.d) -MT $@ $< -o $@

Dict.h Dict.cpp: $(DICTHEADERS) Makefile
	$(ROOTCINT) -f Dict.cpp -c -p $(Includes) $(CINTFLAGS) $(DICTHEADERS) LinkDef.h

## Cleaning rules
clean:
	-$(RM) *.o G__auto*LinkDef.h Dict* *.d

## Include dependence files
SkipDepInclude = no

ifeq ($(MAKECMDGOALS), clean)
SkipDepInclude = yes
endif

ifeq ($(SkipDepInclude), no)
-include *.d
endif
