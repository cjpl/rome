romebuilder: builder/src/ROMEBuilder.cpp src/ROMEXML.cpp src/ROMEString.cpp
	g++ -o $(ROMESYS)/bin/$@ builder/src/ROMEBuilder.cpp src/ROMEXML.cpp src/ROMEString.cpp -I$(ROMESYS)/include/ -I$(ROMESYS)/builder/include/ -lxml2 -lz -lpthread
