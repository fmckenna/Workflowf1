CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include -I ./nanoflann/include
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a

SRCS=createSAM.cpp HazusSAM_Generator.cpp FloorParam.cpp InterstoryParam.cpp Building.cpp
OBJS=createSAM.o HazusSAM_Generator.o FloorParam.o InterstoryParam.o Building.o

all: createSAM

HazusSam_Generator.o: HazusSAM_Generator.cpp HazusSAM_Generator.h

Building.o: Building.cpp Building.h

FloorParam.o: FloorParam.cpp FloorParam.h

InterstoryParam.o: InterstoryParam.cpp InterstoryParam.h

createSAM.o: createSAM.cpp

createSAM: $(OBJS)
	$(CXX) $(LDFLAGS) -o createSAM $(OBJS) $(LDLIBS)

clean:
	$(RM) $(OBJS) *~ \#*

distclean: clean
	$(RM) createSAM


