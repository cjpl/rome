romebuilder: Builder/src/ROMEBuilder.cpp 
	g++ -o $(ROMESYS)/bin/$@ Builder/src/ROMEBuilder.cpp -I$(ROMESYS)/include/ -I$(ROMESYS)/Builder/include/ -lxml2 -lz -lpthread
