#####################################################################
#
#  Name:         Makefile
#  Created by:   Matthias Schneebeli
#
#  $Log$
#  Revision 1.9  2004/10/20 00:18:32  pierre
#  correct Midas support and romebuilder image name
#
#  Revision 1.8  2004/10/19 22:08:27  pierre
#  add MIDAS support
#
#####################################################################
#
INCLUDE :=-I$(ROMESYS)/include/ -I$(ROMESYS)/builder/include/ -I$(ROOTSYS)/include/
rootlib := $(shell root-config --libs)
rootlib2 := -L$(ROOTSYS)/lib/ -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint \
	    -lPostscript -lMatrix -lPhysics -lpthread -lm -ldl -rdynamic
LIBRARY := $(rootlib2) -lxml2 -lz -lpthread

all: prebuilder romebuilder.exe

prebuilder:
ifdef MIDASSYS
	@echo "..."
	@echo ">>>> Midas support included in this ROMEbuilder! <<<<"
	@echo "..."
LIBRARY += -L$(MIDASSYS)/linux/lib -lmidas
CFLAGS := -DHAVE_MIDAS
else
	@echo "..."
	@echo "<<<< No Midas support for this ROMEbuilder! >>>>"
	@echo "..."
endif

romebuilder.exe: builder/src/ROMEBuilder.cpp src/ROMEXML.cpp src/ROMEString.cpp
	g++ $(CFLAGS) -o $(ROMESYS)/bin/$@ builder/src/ROMEBuilder.cpp src/ROMEXML.cpp \
	src/ROMEString.cpp $(INCLUDE) $(LIBRARY)

clean:
	rm -rf $(ROMESYS)/bin/romebuilder.exe
