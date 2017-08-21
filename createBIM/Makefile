CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a

SRCS=csvparser.c createBIM.cpp
OBJS=csvparser.o createBIM.o

all: createBIM

csvParser.o: csvparser.cpp csvparser.h

createBIM.o: createBIM.cpp

createBIM: $(OBJS)
	$(CXX) $(LDFLAGS) -o createBIM $(OBJS) $(LDLIBS)

clean:
	$(RM) $(OBJS) *~ \#*

distclean: clean
	$(RM) createBIM


