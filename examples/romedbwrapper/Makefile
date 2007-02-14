## Compilers
CXX   ?= g++
CC    ?= gcc
FC    ?= g77

-include ../../Makefile.arch

## Compile and link flags
rootlibs  := $(shell $(ROOTSYS)/bin/root-config --libs)
rootcflags:= $(shell $(ROOTSYS)/bin/root-config --cflags)
sqllibs   :=  -lsqlite3
sqlcflags := -DHAVE_SQL -DHAVE_SQLITE3

Flags     := $(oscflags) $(rootcflags) $(sqlcflags) $(daqcflags) -g -Wall -W
Libraries :=
Libraries += $(oslibs) $(sqllibs) $(daqlibs) $(rootlibs) $(clibs)
LDFLAGS   := $(osldflags)
SOFLAGS   := $(ossoflags)

## Include directories
Includes := -I$(ROMESYS)/include/

## Objects
objects += ROMESQLite3.o \
           ROMESQL.o \
           ROMESQLDataBase.o \
           ROMESQLDataBaseWrapper.o \
           ROMEStr2DArray.o \
           ROMEStrArray.o \
           ROMEString.o \
           ROMEPrint.o \
           ROMEPath.o \
           Dict.o \

DICTHEADERS = $(ROMESYS)/include/ROMEString.h \
              $(ROMESYS)/include/ROMEStrArray.h \
              $(ROMESYS)/include/ROMEStr2DArray.h \
              $(ROMESYS)/include/ROMEPrint.h \
              $(ROMESYS)/include/ROMEPath.h \
              $(ROMESYS)/include/ROMESQL.h \
              $(ROMESYS)/include/ROMESQLite3.h \
              $(ROMESYS)/include/ROMESQLDataBase.h \
              $(ROMESYS)/include/ROMEDataBase.h \

##
## Compile and link rules
##
all: test_c test_f

## Link statements
test_c: $(objects) main_c.o
	$(CXX) $(LDFLAGS) -o $@ $(objects) main_c.o $(Libraries)

test_f: $(objects) main_f.o
	$(FC) $(LDFLAGS) -o $@ main_f.o $(objects) $(Libraries)

ROME%.o : $(ROMESYS)/src/ROME%.cpp
	$(CXX) -c $(Flags) $(Includes) -MMD -MP -MF $(@:.o=.d) -MT $@ $< -o $@

main_c.o : main_c.c
	$(CC) -c $(Flags) $(Includes) -MMD -MP -MF $(@:.o=.d) -MT $@ $< -o $@

main_f.o : main_f.F
	$(FC) -c $(Flags) $(Includes) $< -o $@

Dict.o : Dict.cpp Dict.h
	$(CXX) -c $(Flags) $(Includes) -MMD -MP -MF $(@:.o=.d) -MT $@ $< -o $@

Dict.h Dict.cpp: $(DICTHEADERS) Makefile
	$(ROOTSYS)/bin/rootcint -f Dict.cpp -c -p $(Includes) $(DICTHEADERS)

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
