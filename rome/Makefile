#####################################################################
#
#  Name:         Makefile
#  Created by:   Matthias Schneebeli
#
# $Id$
#
#####################################################################

### Switch for creating librome.a
###  If yes, ROME classes are packed in librome.a and linked to each projects
###  instead of compiling in each projects.
###  When you changed this key, you need to do "make clean"
# LIBROME = yes

### Add -g compile option when compiling librome.a
# ROMEDEBUG = yes

### Add -O compile option when compiling librome.a
# ROMEOPTIMIZE = yes

### Add -pg compile option when compiling librome.a
# ROMEPROFILE = yes

### Switch for verbose make. 1 or 0. (Default = 1)
# ROMEVERBOSEMAKE = 0

### Compile option for romebuilder
# ROMEBLD_FLAGS = -g -O

### PIC(position-independent code) option. Default is PIC.
### Valid values are 'PIC', 'pic' or 'no'.
### 'PIC' : use -fPIC option during compile.
### 'pic' : use -fpic option during compile.
### 'no'  : not use -fPIC or -pfic.
### -fPIC or -fpic is necessary when you make dynamic link library.
### -fpic is slightly faster than -fPIC, but all operating system don't support.
### For details, please read man page of gcc.
# ROMEPIC = no

### ROMESYS
ROMESYS = $(shell pwd)

### Compiler
CXX   ?= g++
CC    ?= gcc
CXXLD ?= $(CXX)

### Additional flags
# ROMECFLAGS = -W -Wall -pipe
# ROMECXXFLAGS = -W -Wall -pipe
# ROMELDFLAGS = -W -Wall -pipe

#####################################################################
# Nothing needs to be modified after this line 
#####################################################################
ifdef ROOTSYS
ROOTCONFIG := $(ROOTSYS)/bin/root-config
ROOTCINT   := $(ROOTSYS)/bin/rootcint
else
ROOTCONFIG := root-config
ROOTCINT   := rootcint
endif

INCLUDE := -Iinclude/ -Iargus/include/ -Ibuilder/include/ $(shell $(ROOTCONFIG) --cflags)
LIBRARY := $(shell $(ROOTCONFIG) --glibs) -lHtml -lThread
TARGET :=  obj include/ROMEVersion.h bin/romebuilder.exe bin/rome-config bin/hadd

# Required ROOT version
ROOT_MAJOR_MIN = 4
ROOT_MINOR_MIN = 02
ROOT_PATCH_MIN = 00

# Local ROOT version
ROOT_VERSION := $(shell $(ROOTCONFIG) --version)
ROOT_MAJOR := $(shell $(ROOTCONFIG) --version 2>&1 | cut -d'.' -f1)
ROOT_MINOR := $(shell $(ROOTCONFIG) --version 2>&1 | cut -d'/' -f1 | cut -d'.' -f2)
ROOT_PATCH := $(shell $(ROOTCONFIG) --version 2>&1 | cut -d'/' -f2)

# Local compiler version
GCC_VERSION := $(shell $(CXX) -dumpversion)
GCC_MAJOR := $(shell $(CXX) -dumpversion 2>&1 | cut -d'.' -f1)
GCC_MINOR := $(shell $(CXX) -dumpversion 2>&1 | cut -d'.' -f2)
GCC_PATCH := $(shell $(CXX) -dumpversion 2>&1 | cut -d'.' -f3)

ROOT_VERSION_ERROR := no
ifeq ($(shell expr $(ROOT_MAJOR) \< $(ROOT_MAJOR_MIN)), 1)
  ROOT_VERSION_ERROR := yes
else
  ifeq ($(ROOT_MAJOR), $(ROOT_MAJOR_MIN))
    ifeq ($(shell expr $(ROOT_MINOR) \< $(ROOT_MINOR_MIN)), 1)
      ROOT_VERSION_ERROR := yes
    else
      ifeq ($(ROOT_MINOR), $(ROOT_MINOR_MIN))
        ifeq ($(shell expr $(ROOT_PATCH) \< $(ROOT_PATCH_MIN)), 1)
          ROOT_VERSION_ERROR := yes
        endif
      endif
    endif
  endif
endif

# reset when ROMEDEBUG, ROMEOPTIMIZE or ROMEPROFILE are yes
ifeq ($(ROMEDEBUG), yes)
  ROMELIB_FLAGS += -g
endif
ifeq ($(ROMEOPTIMIZE), yes)
  ROMELIB_FLAGS += -O
endif
ifeq ($(ROMEPROFILE), yes)
  ROMELIB_FLAGS += -pg
endif

CFLAGS += $(ROMECFLAGS) $(OSCFLAGS)
CXXFLAGS += $(ROMECXXFLAGS) $(OSCXXFLAGS)
LDFLAGS += $(ROMELDFLAGS) $(OSLDFLAGS)

# -Wno-unused-but-set is available from GCC 4.5.3
ifeq ($(shell expr $(GCC_MAJOR) \> 4), 1)
  CFLAGS   += -Wno-unused-but-set-variable
  CXXFLAGS += -Wno-unused-but-set-variable
else
  ifeq ($(GCC_MAJOR), 4)
    ifeq ($(shell expr $(GCC_MINOR) \> 4), 1)
      CFLAGS   += -Wno-unused-but-set-variable
      CXXFLAGS += -Wno-unused-but-set-variable
    endif
  endif
endif

ifeq ($(ROMEPIC), pic)
ROMEPICOPT = -fpic
ROMEPICDEF = -DUSE_PIC_LOWER
else
ifeq ($(ROMEPIC), no)
ROMEPICOPT =
ROMEPICDEF =
else
ROMEPICOPT = -fPIC
ROMEPICDEF = -DUSE_PIC_UPPER
endif
endif

DICTIONARIES = ROMEBuilderDict.h UpdateVersionHDict.h HAddDict.h

NEED_TARRAYL64 = no
ifeq ($(ROOT_MAJOR), 5)
ifeq ($(shell expr $(ROOT_MINOR) \< 14), 1)
NEED_TARRAYL64 = yes
endif
endif
ifeq ($(ROOT_MAJOR), 4)
NEED_TARRAYL64 = yes
endif

ifeq ($(NEED_TARRAYL64), yes)
INCLUDE += -DNEED_TARRAYL64 -Iinclude/array64
endif

ROMEVERBOSEMAKE ?= 1
ifeq ($(ROMEVERBOSEMAKE), 0)
   Q = @
   romeechoing = @echo $1
else
   Q =
   romeechoing =
endif

-include Makefile.arch

ifeq ($(LIBROME), yes)
  INCLUDE += -DHAVE_LIBROME
  LIBROMEFILE := librome.a
  DICTIONARIES += ROMELibDict.h
  TARGET += $(LIBROMEFILE)
else
  LIBROMEFILE =
endif

BldObjects := obj/ROMEBuilder.o \
              obj/ROMEBuildReadXML.o \
              obj/ROMEBuildWriteCode.o \
              obj/ROMEBuildWriteCode2.o \
              obj/ROMEBuildMakeFile.o \
              obj/ROMEString.o \
              obj/ROMEStrArray.o \
              obj/ROMEStr2DArray.o \
              obj/ROMEXML.o \
              obj/ROMEPrint.o \
              obj/ROMEBuilderDict.o \
              obj/ROMEConfigParameter.o \
              obj/mxml.o \
              obj/strlcpy.o

BldDictHeaders := include/ROMEString.h \
                  include/ROMEStrArray.h \
                  include/ROMEStr2DArray.h \
                  include/ROMEXML.h

HAddObjects := obj/ROMETGraph.o \
               obj/HAddDict.o
HAddDictHeaders := include/ROMETGraph.h

UpHObjects := obj/ROMEString.o \
              obj/ROMEStrArray.o \
              obj/ROMEXML.o \
              obj/ROMEPrint.o \
              obj/mxml.o \
              obj/strlcpy.o \
              obj/UpdateVersionHDict.o

UpHDictHeaders := include/ROMEString.h \
                  include/ROMEStrArray.h \
                  include/ROMEXML.h \

LibObjects := obj/ROMEStr2DArray.o \
              obj/ROMEStrArray.o \
              obj/ROMEString.o \
              obj/ROMEXML.o \
              obj/ROMELibDict.o \
              obj/ROMEConfigToForm.o \
              obj/ROMEDAQSystem.o \
              obj/ROMEDataBaseDAQ.o \
              obj/ROMERootDAQ.o \
              obj/ROMEConfig.o \
              obj/ROMENetFolder.o \
              obj/ROMENetFolderServer.o \
              obj/ROMEODBOfflineDataBase.o \
              obj/ROMEOrcaDAQ.o \
              obj/ROMEPath.o \
              obj/ROMERomeDAQ.o \
              obj/ROMETask.o \
              obj/ROMETextDataBase.o \
              obj/ROMEUtilities.o \
              obj/ROMEXMLDataBase.o \
              obj/ROMETree.o \
              obj/ROMEGraph.o \
              obj/ROMELabel.o \
              obj/ROMECompositeFrame.o \
              obj/ROMEHisto.o \
              obj/ROMETGraph.o \
              obj/ROMETGraphErrors.o \
              obj/ROMETCutG.o \
              obj/TNetFolder.o \
              obj/TNetFolderServer.o \
              obj/XMLToForm.o \
              obj/XMLToFormWindow.o \
              obj/XMLToFormFrame.o \
              obj/XMLToFormElement.o \
              obj/ArgusAnalyzerController.o \
              obj/ArgusTab.o \
              obj/ArgusHistoDisplay.o \
              obj/ArgusTextDialog.o \
              obj/ArgusWindow.o \
              obj/mxml.o \
              obj/strlcpy.o
ifeq ($(NEED_TARRAYL64), yes)
LibObjects += obj/TArrayL64.o
endif

LibDictHeaders := include/ROMEString.h \
                  include/ROMEStrArray.h \
                  include/ROMEStr2DArray.h \
                  include/ROMEXML.h \
                  include/ROMETask.h \
                  include/ROMETreeInfo.h \
                  include/ROMENetFolder.h \
                  include/ROMENetFolderServer.h \
                  include/ROMEConfig.h \
                  include/ROMEConfigGraph.h \
                  include/ROMEDataBase.h \
                  include/ROMENoDataBase.h \
                  include/ROMEODBOfflineDataBase.h \
                  include/ROMETextDataBase.h \
                  include/ROMEXMLDataBase.h \
                  include/ROMETree.h \
                  include/ROMEGraph.h \
                  include/ROMEPath.h \
                  include/ROMEOrcaDAQ.h \
                  include/ROMEStopwatch.h \
                  include/ROMEDAQSystem.h \
                  include/ROMEUtilities.h \
                  include/ROMEConfigToForm.h \
                  include/ROMEDataBaseDAQ.h \
                  include/ROMERootDAQ.h \
                  include/ROMENoDAQSystem.h \
                  include/ROMERomeDAQ.h \
                  include/ROMEDataBase.h \
                  include/ROMELabel.h \
                  include/ROMECompositeFrame.h \
                  include/ROMEHisto.h \
                  include/ROMEConfigHisto.h \
                  include/ROMETGraph.h \
                  include/ROMETGraphErrors.h \
                  include/ROMETCutG.h \
                  include/TNetFolder.h \
                  include/TNetFolderServer.h \
                  include/XMLToFormWindow.h \
                  include/XMLToForm.h \
                  include/XMLToFormElement.h \
                  include/XMLToFormElementSignal.h \
                  include/XMLToFormFrame.h \
                  include/XMLToFormWindow.h \
                  argus/include/ArgusWindow.h \
                  argus/include/ArgusTextDialog.h \
                  argus/include/ArgusAnalyzerController.h \
                  argus/include/ArgusTab.h \
                  argus/include/ArgusHistoDisplay.h
ifeq ($(NEED_TARRAYL64), yes)
LibDictHeaders += include/array64/TArrayL64.h
endif

ifeq ($(OSTYPE),darwin)
  export DYLD_LIBRARY_PATH += :$(shell $(ROOTCONFIG) --libdir)
else
  export LD_LIBRARY_PATH += :$(shell $(ROOTCONFIG) --libdir)
endif

#
# Compile and linking
#

all: checkenv-rootversion $(TARGET)
	@./bin/updateVersionH.exe

obj:
	@if [ ! -d  obj ] ; then \
		echo "Making directory obj" ; \
		mkdir obj; \
	fi;

dict: $(DICTIONARIES)

bin/romebuilder.exe: builder/src/main.cpp include/ROMEVersion.h $(BldObjects)
	$(call romeechoing, "linking   $@")
	$(Q)$(CXXLD) $(LDFLAGS) $(ROMEBLD_FLAGS) $(INCLUDE) -o $@ $< $(BldObjects) $(LIBRARY)

bin/updateVersionH.exe: tools/UpdateVersionH/main.cpp  $(UpHObjects)
	$(call romeechoing, "linking   $@")
	$(Q)$(CXXLD) $(LDFLAGS) $(INCLUDE) -g -o $@ $< $(UpHObjects) $(LIBRARY)

bin/rome-config: tools/rome-config/main.cpp include/ROMEVersion.h
	$(call romeechoing, "linking   $@")
	$(Q)$(CXXLD) $(LDFLAGS) -W -Wall $(INCLUDE) -o $@ $< $(LIBRARY)

bin/hadd: tools/hadd/hadd.cxx $(HAddObjects)
	$(call romeechoing, "linking   $@")
	$(Q)$(CXXLD) $(LDFLAGS) -W -Wall $(INCLUDE) -o $@ $< $(HAddObjects) $(LIBRARY)

include/ROMEVersion.h: bin/updateVersionH.exe
	$(call romeechoing, "creating  $@")
	@./bin/updateVersionH.exe

librome.a: Makefile $(LibObjects)
	$(call romeechoing, "creating  $@")
	-$(Q)$(RM) $@
	$(Q)$(AR) -rcs $@ $(LibObjects)

librome.so: Makefile $(LibObjects)
	$(call romeechoing, "creating  $@")
ifeq ($(OSTYPE),darwin)
	$(Q)$(CXXLD) $(ROMELIB_FLAGS) $(SOFLAGS) -o $(ROMESYS)/librome.dylib $(LibObjects)
	-$(Q)$(RM) librome.so
	$(Q)ln -s librome.dylib librome.so
else
	$(Q)$(CXXLD) $(ROMELIB_FLAGS) $(SOFLAGS) -o $(ROMESYS)/librome.so $(LibObjects)
endif

ROMELibDict.h ROMELibDict.cpp: $(LibDictHeaders) Makefile
	$(call romeechoing, "creating  $@")
	$(Q)$(ROOTCINT) -f ROMELibDict.cpp -c -p $(INCLUDE) $(CINTFLAGS) $(LibDictHeaders) include/ROMELibLinkDef.h

ROMEBuilderDict.h ROMEBuilderDict.cpp: $(BldDictHeaders) Makefile
	$(call romeechoing, "creating  $@")
	$(Q)$(ROOTCINT) -f ROMEBuilderDict.cpp -c -p $(INCLUDE) $(CINTFLAGS) $(BldDictHeaders) include/ROMEBuildLinkDef.h

UpdateVersionHDict.h UpdateVersionHDict.cpp: $(UpHDictHeaders) Makefile
	$(call romeechoing, "creating  $@")
	$(Q)$(ROOTCINT) -f UpdateVersionHDict.cpp -c -p $(INCLUDE) $(CINTFLAGS) $(UpHDictHeaders) include/UpdateVersionHLinkDef.h

HAddDict.h HAddDict.cpp: $(HAddDictHeaders) Makefile
	$(call romeechoing, "creating  $@")
	$(Q)$(ROOTCINT) -f HAddDict.cpp -c -p $(INCLUDE) $(CINTFLAGS) $(HAddDictHeaders) include/HAddLinkDef.h

obj/mxml.o: src/mxml.c include/mxml.h
	$(call romeechoing, "compiling $@")
	$(Q)$(CC) $(CFLAGS) $(ROMELIB_FLAGS) $(ROMEPICOPT) $(INCLUDE) -MMD -MP -MF $(@:.o=.d) -MT $@ -c -o $@ $<

obj/strlcpy.o: src/strlcpy.c include/strlcpy.h
	$(call romeechoing, "compiling $@")
	$(Q)$(CC) $(CFLAGS) $(ROMELIB_FLAGS) $(ROMEPICOPT) $(INCLUDE) -MMD -MP -MF $(@:.o=.d) -MT $@ -c -o $@ $<

obj/TArrayL64.o: src/TArrayL64.cpp include/array64/TArrayL64.h
	$(call romeechoing, "compiling $@")
	$(Q)$(CXX) $(CXXFLAGS) $(ROMELIB_FLAGS) $(ROMEPICOPT) $(INCLUDE) -MMD -MP -MF $(@:.o=.d) -MT $@ -c -o $@ $<

obj/%Dict.o: %Dict.cpp %Dict.h
	$(call romeechoing, "compiling $@")
	$(Q)$(CXX) $(CXXFLAGS) $(ROMEPICOPT) -O0 -MMD -MP -MF $(@:.o=.d) -MT $@  -c $(INCLUDE) -o $@ $<

obj/ROMEBuild%.o: builder/src/ROMEBuild%.cpp builder/include/ROMEBuilder.h
	$(call romeechoing, "compiling $@")
	$(Q)$(CXX) $(CXXFLAGS) $(ROMEBLD_FLAGS) $(ROMEPICDEF) $(INCLUDE) -MMD -MP -MF $(@:.o=.d) -MT $@ -c -o $@ $<

obj/ROMEBuildWriteCode%.o: builder/src/ROMEBuildWriteCode%.cpp builder/include/ROMEBuilder.h $(LIBROMEFILE)
	$(call romeechoing, "compiling $@")
	$(Q)$(CXX) $(CXXFLAGS) $(ROMEBLD_FLAGS) $(ROMEPICDEF) $(INCLUDE) -MMD -MP -MF $(@:.o=.d) -MT $@ -c -o $@ $<

obj/ROMEConfigParameter.o: builder/src/ROMEConfigParameter.cpp builder/include/ROMEConfigParameter.h
	$(call romeechoing, "compiling $@")
	$(Q)$(CXX) $(CXXFLAGS) $(ROMELIB_FLAGS) $(ROMEPICOPT) $(INCLUDE) -MMD -MP -MF $(@:.o=.d) -MT $@ -c -o $@ $<

obj/Argus%.o: argus/src/Argus%.cpp argus/include/Argus%.h
	$(call romeechoing, "compiling $@")
	$(Q)$(CXX) $(CXXFLAGS) $(ROMELIB_FLAGS) $(ROMEPICOPT) $(INCLUDE) -MMD -MP -MF $(@:.o=.d) -MT $@ -c -o $@ $<

obj/%.o: src/%.cpp include/%.h
	$(call romeechoing, "compiling $@")
	$(Q)$(CXX) $(CXXFLAGS) $(ROMELIB_FLAGS) $(ROMEPICOPT) $(INCLUDE) -MMD -MP -MF $(@:.o=.d) -MT $@ -c -o $@ $<



clean:
	-$(RM) $(BldObjects) $(UpHObjects) $(HAddObjects) $(LibObjects) \
	ROMELibDict.h ROMELibDict.cpp \
	ROMEBuilderDict.h ROMEBuilderDict.cpp \
	UpdateVersionHDict.h UpdateVersionHDict.cpp \
	HAddDict.h HAddDict.cpp
	-find . -maxdepth 1 -name "G__auto*LinkDef.h" | xargs $(RM)
	-find obj -name "*.d" | xargs $(RM)
	-find obj -name "*.o" | xargs $(RM)

SkipDepInclude = no

ifeq ($(MAKECMDGOALS), clean)
SkipDepInclude = yes
endif

ifeq ($(SkipDepInclude), no)
-include obj/*.d
endif

checkenv-rootversion:
ifeq ($(ROOT_VERSION_ERROR), yes)
	@echo ROOT $(ROOT_VERSION) is not supported.
	@echo Please update to $(ROOT_MAJOR_MIN).$(ROOT_MINOR_MIN)/$(ROOT_PATCH_MIN) or later
	@exit 1
endif
