INCLUDE :=-I$(ROMESYS)/include/ -I$(ROMESYS)/builder/include/ -I$(ROOTSYS)/include/
rootlib := $(shell root-config --libs)
rootlib2 := -L/cern/root/lib/ -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lpthread -lm -ldl -rdynamic
LIBRARY := $(rootlib2) -lxml2 -lz -lpthread

romebuilder: builder/src/ROMEBuilder.cpp src/ROMEXML.cpp src/ROMEString.cpp
	g++ -o $(ROMESYS)/bin/$@ builder/src/ROMEBuilder.cpp src/ROMEXML.cpp src/ROMEString.cpp $(INCLUDE) $(LIBRARY)
