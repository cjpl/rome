#####################################################################
#
#  Name:         Makefile
#  Created by:   Matthias Schneebeli
#
#  $Log$
#  Revision 1.12  2004/12/02 17:46:43  sawada
#  Macintosh port
#
#  Revision 1.11  2004/10/25 08:34:06  schneebeli_m
#  *** empty log message ***
#
#  Revision 1.10  2004/10/21 13:03:51  schneebeli_m
#  *** empty log message ***
#
#  Revision 1.9  2004/10/20 00:18:32  pierre
#  correct Midas support and romebuilder image name
#
#  Revision 1.8  2004/10/19 22:08:27  pierre
#  add MIDAS support
#
#####################################################################
#
INCLUDE :=  -I$(ROMESYS)/include/ -I$(ROMESYS)/builder/include/ $(shell root-config --cflags) $(shell xml2-config --cflags)
LIBRARY := $(shell root-config --libs) $(shell xml2-config --libs) -lpthread -lg2c

## for Macintosh
ifeq ($(shell uname),Darwin)
FINK_DIR := $(shell which fink 2>&1 | sed -ne "s/\/bin\/fink//p")
Include += -DHAVE_STRLCPY $(shell [ -d $(FINK_DIR)/include ] && echo -I$(FINK_DIR)/include)
LIBRARY += -multiply_defined suppress $(shell [ -d $(FINK_DIR)/lib ] && echo -L$(FINK_DIR)/lib)
endif

romebuilder.exe: builder/src/ROMEBuilder.cpp src/ROMEXML.cpp src/ROMEString.cpp
	g++ $(CFLAGS) -o $(ROMESYS)/bin/$@ builder/src/ROMEBuilder.cpp src/ROMEXML.cpp \
	src/ROMEString.cpp $(INCLUDE) $(LIBRARY)

clean:
	rm -rf $(ROMESYS)/bin/romebuilder.exe
