romebuilder: builder/src/ROMEBuilder.cpp 
	g++ -o $(ROMESYS)/bin/$@ builder/src/ROMEBuilder.cpp -I$(ROMESYS)/include/ -I$(ROMESYS)/builder/include/ -lxml2 -lz -lpthread
