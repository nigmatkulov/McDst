# Define compiler
CXX = g++

# Define flags
CXXFLAGS = $(shell root-config --cflags) -fPIC -W -Woverloaded-virtual -Wno-deprecated-declarations
CXXFLAGS += -Wall -pipe -std=c++11 -D__ROOT__ -I.
LIBS = $(shell root-config --glibs) -lEG
INCS = $(shell root-config --incdir)

# Define output library
MCDST = libMcDst.so

# Compile all *.cxx classes in the directory
SRC = $(shell find . -name "*.cxx")

all: CXXFLAGS += -O2
all: $(MCDST)

.PHONY: optdebug debug
optdebug: CXXFLAGS += -O2 -g
optdebug: $(MCDST)
debug: CXXFLAGS += -O0 -g
debug: $(MCDST)

# $(SRC:.cc=.o)
$(MCDST): $(SRC:.cxx=.o) McDst_Dict.C
	$(CXX) $(CXXFLAGS) -shared $^ -o $(MCDST) $(LIBS)

%.o: %.cxx
	$(CXX) -fPIC $(CXXFLAGS) -c -o $@ $<

# Dictionary deneration: -DROOT_CINT -D__ROOT__
McDst_Dict.C: $(shell find . -name "*.h" ! -name "*LinkDef*")
	rootcint -f $@ -c -D__ROOT__ -I. -I$(INCS) $^ McDstLinkDef.h

.PHONY: clean distclean converters converters_debug converters_optdebug

clean:
	rm -vf *.o McDst_Dict*

distclean:
	rm -vf *.o McDst_Dict* $(MCDST) urqmd2mc

converters_debug: CXXFLAGS += -O0 -g
converters_debug: converters
converters_optdebug: CXXFLAGS += -O2 -g
converters_optdebug: converters
converters: urqmd2mc pythia8
urqmd2mc: urqmd2mc.cpp
	$(CXX) $(CXXFLAGS) -I$(INCS) $^ -o $(patsubst %.cpp,%,$<) -L. -l$(patsubst lib%.so,%,$(MCDST)) $(LIBS)
pythia8: pythia8gen.cpp
	$(CXX) $(CXXFLAGS) -I$(INCS) $(shell pythia8-config --cflags) $^ -o $(patsubst %.cpp,%,$<) -L. -l$(patsubst lib%.so,%,$(MCDST)) $(shell pythia8-config --libs) $(LIBS)
