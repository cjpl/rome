romebuilder: builder/src/ROMEBuilder.cpp src/ROMEXML.cpp
	g++ -o $(ROMESYS)/bin/$@ builder/src/ROMEBuilder.cpp src/ROMEXML.cpp -I$(ROMESYS)/include/ -I$(ROMESYS)/builder/include/ -lxml2 -lz -lpthread
