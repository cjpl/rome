#####################################################################
#
#  Name:         Makefile
#  Created by:   Matthias Schneebeli
#
# $Id$
#
#####################################################################

# Switch for creating librome.a
#  If yes, ROME classes are packed in librome.a and linked to each projects
#  instead of compiling in each projects.
#  When you changed this key, you need to do "make clean"
# LIBROME = yes

# Add -g compile option when compiling librome.a and romebuilder
# ROMEDEBUG = yes

# Add -O compile option when compiling librome.a and romebuilder
# ROMEOPTIMIZE = yes

# Compiler
CXX ?= g++

#####################################################################
# Nothing needs to be modified after this line 
#####################################################################
INCLUDE := -Iinclude/ -Iargus/include/ -Ibuilder/include/ $(shell $(ROOTSYS)/bin/root-config --cflags)
LIBRARY := $(shell $(ROOTSYS)/bin/root-config --glibs) -lHtml
TARGET :=  obj include/ROMEVersion.h bin/romebuilder.exe

ifeq ($(ROMEDEBUG), yes)
  OPT += -g
endif

ifeq ($(ROMEOPTIMIZE), yes)
  OPT += -O
endif

ifeq ($(LIBROME), yes)
  INCLUDE += -DHAVE_LIBROME
  TARGET += librome.a
endif

-include Makefile.arch

BldObjects := obj/ROMEBuilder.o \
              obj/ROMEBuildReadXML.o \
              obj/ROMEBuildWriteCode.o \
              obj/ROMEBuildMakeFile.o \
              obj/ROMEStr2DArray.o \
              obj/ROMEStrArray.o \
              obj/ROMEPrint.o \
              obj/ROMEString.o \
              obj/ROMEXML.o \
              obj/ROMEBuilderDict.o \
              obj/ROMEConfigParameter.o \
              obj/mxml.o \
              obj/strlcpy.o

BldDictHeaders := include/ROMEString.h \
                  include/ROMEPrint.h \
                  include/ROMEStrArray.h \
                  include/ROMEStr2DArray.h \
                  include/ROMEXML.h

UpHObjects := obj/ROMEString.o \
              obj/ROMEXML.o \
              obj/ROMEPrint.o \
              obj/mxml.o \
              obj/strlcpy.o \
              obj/UpdateVersionHDict.o

UpHDictHeaders := include/ROMEString.h \
                  include/ROMEXML.h \
                  include/ROMEPrint.h

LibObjects := obj/ROMEStr2DArray.o \
              obj/ROMEStrArray.o \
              obj/ROMEString.o \
              obj/ROMEPrint.o \
              obj/ROMEXML.o \
              obj/ROMELibDict.o \
              obj/ROMEConfigToForm.o \
              obj/ROMEDAQSystem.o \
              obj/ROMEDataBaseDAQ.o \
              obj/ROMERootDAQ.o \
              obj/ROMENetFolder.o \
              obj/ROMENetFolderServer.o \
              obj/ROMEODBOfflineDataBase.o \
              obj/ROMEODBOnlineDataBase.o \
              obj/ROMEOrcaDAQ.o \
              obj/ROMEPath.o \
              obj/ROMERomeDAQ.o \
              obj/ROMETask.o \
              obj/ROMERint.o \
              obj/ROMETextDataBase.o \
              obj/ROMEUtilities.o \
              obj/ROMEXMLDataBase.o \
              obj/ROMETree.o \
              obj/ROMELabel.o \
              obj/ROMEHorizontalFrame.o \
              obj/TArrayL64.o \
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

LibDictHeaders := include/ROMEString.h \
                  include/ROMEStrArray.h \
                  include/ROMEStr2DArray.h \
                  include/ROMEPrint.h \
                  include/ROMEXML.h \
                  include/ROMETask.h \
                  include/ROMERint.h \
                  include/ROMETreeInfo.h \
                  include/ROMENetFolder.h \
                  include/ROMENetFolderServer.h \
                  include/ROMEConfig.h \
                  include/ROMEDataBase.h \
                  include/ROMENoDataBase.h \
                  include/ROMEODBOfflineDataBase.h \
                  include/ROMEODBOnlineDataBase.h \
                  include/ROMETextDataBase.h \
                  include/ROMEXMLDataBase.h \
                  include/ROMETree.h \
                  include/ROMEPath.h \
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
                  include/ROMEHorizontalFrame.h \
                  include/TNetFolder.h \
                  include/TNetFolderServer.h \
                  include/XMLToFormWindow.h \
                  include/XMLToForm.h \
                  include/XMLToFormElement.h \
                  include/XMLToFormElementSignal.h \
                  include/XMLToFormFrame.h \
                  include/XMLToFormWindow.h \
                  include/TArrayL64.h \
                  argus/include/ArgusWindow.h \
                  argus/include/ArgusTextDialog.h \
                  argus/include/ArgusAnalyzerController.h \
                  argus/include/ArgusTab.h \
                  argus/include/ArgusHistoDisplay.h

#
# Compile and linking
#

all: $(TARGET)
	@./bin/updateVersionH.exe

obj:
	@if [ ! -d  obj ] ; then \
		echo "Making directory obj" ; \
		mkdir obj; \
	fi;

bin/romebuilder.exe: builder/src/main.cpp $(BldObjects)
	$(CXX) $(OPT) $(CFLAGS) $(INCLUDE) -o $@ $< $(BldObjects) $(LIBRARY)

bin/updateVersionH.exe: tools/UpdateVersionH/main.cpp  $(UpHObjects)
	$(CXX) $(OPT) $(CFLAGS) $(INCLUDE) -o $@ $< $(UpHObjects) $(LIBRARY)

include/ROMEVersion.h: bin/updateVersionH.exe
	@./bin/updateVersionH.exe

librome.a: $(LibObjects)
	-$(RM) $@
	$(AR) -cr $@ $^

obj/mxml.o: src/mxml.c include/mxml.h
	$(CXX) $(OPT) $(CFLAGS) $(INCLUDE) -c -o $@ $<

obj/strlcpy.o: src/strlcpy.c include/strlcpy.h
	$(CXX) $(OPT) $(CFLAGS) $(INCLUDE) -c -o $@ $<

obj/%Dict.o: %Dict.cpp %Dict.h
	$(CXX) $(CFLAGS) -c $(INCLUDE) -o $@ $<

ROMELibDict.h ROMELibDict.cpp: $(LibDictHeaders)
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) \
	$(ROOTSYS)/bin/rootcint -f ROMELibDict.cpp -c -p $(CFLAGS) $(INCLUDE) $(LibDictHeaders)

ROMEBuilderDict.h ROMEBuilderDict.cpp: $(BldDictHeaders)
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) \
	$(ROOTSYS)/bin/rootcint -f ROMEBuilderDict.cpp -c -p $(CFLAGS) $(INCLUDE) $(BldDictHeaders)

UpdateVersionHDict.h UpdateVersionHDict.cpp: $(UpHDictHeaders)
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) \
	$(ROOTSYS)/bin/rootcint -f UpdateVersionHDict.cpp -c -p $(CFLAGS) $(INCLUDE) $(UpHDictHeaders)

obj/ROMEBuild%.o: builder/src/ROMEBuild%.cpp builder/include/ROMEBuilder.h
	$(CXX) $(OPT) $(CFLAGS) $(INCLUDE) -c -o $@ $<

obj/ROMEConfigParameter.o: builder/src/ROMEConfigParameter.cpp builder/include/ROMEConfigParameter.h
	$(CXX) $(OPT) $(CFLAGS) $(INCLUDE) -c -o $@ $<

obj/Argus%.o: argus/src/Argus%.cpp argus/include/Argus%.h
	$(CXX) $(OPT) $(CFLAGS) $(INCLUDE) -c -o $@ $<

obj/%.o: src/%.cpp include/%.h
	$(CXX) $(OPT) $(CFLAGS) $(INCLUDE) -c -o $@ $<

clean:
	-$(RM) $(BldObjects) $(UpHObjects) $(LibObjects) \
	ROMELibDict.h ROMELibDict.cpp \
	ROMEBuilderDict.h ROMEBuilderDict.cpp \
	UpdateVersionDictH.h UpdateVersionDictH.cpp
