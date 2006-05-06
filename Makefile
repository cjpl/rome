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
LIBROME = no

# Compile mode for librome.a and romebuilder
ROMEDEBUG = no

#####################################################################
# Nothing needs to be modified after this line 
#####################################################################
INCLUDE := -I. -Iinclude/ -Iargus/include/ -Ibuilder/include/ $(shell $(ROOTSYS)/bin/root-config --cflags)
LIBRARY := $(shell $(ROOTSYS)/bin/root-config --glibs) -lHtml
TARGET :=  obj bin/romebuilder.exe

ifeq ($(ROMEDEBUG), yes)
  OPT = -g
else
  OPT = -O2
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
              obj/ROMEString.o \
              obj/ROMEXML.o \
              obj/ROMEBuilderDict.o \
              obj/mxml.o \
              obj/strlcpy.o

BldDictHeaders := include/ROMEString.h \
                  include/ROMEStrArray.h \
                  include/ROMEStr2DArray.h \
                  include/ROMEXML.h

LibObjects := obj/ROMEStr2DArray.o \
              obj/ROMEStrArray.o \
              obj/ROMEString.o \
              obj/ROMEXML.o \
              obj/ROMELibDict.o \
              obj/ROMEConfigToForm.o \
              obj/ROMEDAQSystem.o \
              obj/ROMEDataBaseDAQ.o \
              obj/ROMENetFolder.o \
              obj/ROMENetFolderServer.o \
              obj/ROMEODBOfflineDataBase.o \
              obj/ROMEODBOnlineDataBase.o \
              obj/ROMEOrcaDAQ.o \
              obj/ROMEPath.o \
              obj/ROMERomeDAQ.o \
              obj/ROMETask.o \
              obj/ROMETextDataBase.o \
              obj/ROMEUtilities.o \
              obj/ROMEXMLDataBase.o \
              obj/TArrayL64.o \
              obj/TNetFolder.o \
              obj/TNetFolderServer.o \
              obj/XMLToForm.o \
              obj/XMLToFormWindow.o \
              obj/ArgusAnalyzerController.o \
              obj/ArgusTab.o \
              obj/ArgusTextDialog.o \
              obj/ArgusWindow.o \
              obj/mxml.o \
              obj/strlcpy.o

LibDictHeaders := include/ROMEString.h \
                  include/ROMEStrArray.h \
                  include/ROMEStr2DArray.h \
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
                  include/ROMEConfigToFormElements.h \
                  include/ROMEDataBaseDAQ.h \
                  include/ROMENoDAQSystem.h \
                  include/ROMERomeDAQ.h \
                  include/ROMEDataBase.h \
                  include/TNetFolder.h \
                  include/TNetFolderServer.h \
                  include/XMLToFormWindow.h \
                  include/XMLToForm.h \
                  include/XMLToFormFrame.h \
                  include/XMLToFormWindow.h \
                  include/TArrayL64.h \
                  argus/include/ArgusWindow.h \
                  argus/include/ArgusTextDialog.h \
                  argus/include/ArgusAnalyzerController.h \
                  argus/include/ArgusTab.h

#
# Compile and linking
#

all: $(TARGET)

obj:
	@if [ ! -d  obj ] ; then \
		echo "Making directory obj" ; \
		mkdir obj; \
	fi;

bin/romebuilder.exe: builder/src/main.cpp $(BldObjects)
	g++ $(OPT) $(CFLAGS) -g  $(INCLUDE) -o $@ $< $(BldObjects) $(LIBRARY)

librome.a: $(LibObjects)
	rm -f $@
	ar -cr $@ $^

obj/mxml.o: src/mxml.c include/mxml.h
	g++ $(OPT) $(CFLAGS) -g  $(INCLUDE) -c -o $@ $<

obj/strlcpy.o: src/strlcpy.c include/strlcpy.h
	g++ $(OPT) $(CFLAGS) -g  $(INCLUDE) -c -o $@ $<

obj/%Dict.o: %Dict.cpp %Dict.h
	g++ $(CFLAGS) -c $(INCLUDE) -o $@ $<

ROMELibDict.h ROMELibDict.cpp: $(LibDictHeaders) include/LinkDef.h
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) $(ROOTSYS)/bin/rootcint -f ROMELibDict.cpp -c -p $(CFLAGS) $(INCLUDE) $(LibDictHeaders) include/LinkDef.h

ROMEBuilderDict.h ROMEBuilderDict.cpp: $(BldDictHeaders)
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) $(ROOTSYS)/bin/rootcint -f ROMEBuilderDict.cpp -c -p $(CFLAGS) $(INCLUDE) $(BldDictHeaders)

obj/ROMEBuild%.o: builder/src/ROMEBuild%.cpp builder/include/ROMEBuilder.h
	g++ $(OPT) $(CFLAGS) -g  $(INCLUDE) -c -o $@ $<

obj/Argus%.o: argus/src/Argus%.cpp argus/include/Argus%.h
	g++ $(OPT) $(CFLAGS) -g  $(INCLUDE) -c -o $@ $<

obj/%.o: src/%.cpp include/%.h
	g++ $(OPT) $(CFLAGS) -g  $(INCLUDE) -c -o $@ $<

clean:
	-rm -f $(BldObjects) $(LibObjects) ROMELibDict.h ROMELibDict.cpp ROMEBuilderDict.h ROMEBuilderDict.cpp
