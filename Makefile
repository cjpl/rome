INCLUDE :=-I$(ROMESYS)/include/ -I$(ROMESYS)/builder/include/ -I$(ROOTSYS)/include/
LIBRARY := -lxml2 -lz -lpthread -lCore

romebuilder: builder/src/ROMEBuilder.cpp src/ROMEXML.cpp src/ROMEString.cpp
	g++ -o $(ROMESYS)/bin/$@ builder/src/ROMEBuilder.cpp src/ROMEXML.cpp src/ROMEString.cpp $(INCLUDE) $(LIBRARY)
