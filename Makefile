# In order to activate pythia8gen, which perfroms conversion
# from the pythia8 output to McDst format, one needs to
# have pythia8 compiled and included in the environment variables
# in order to have Pythia/Pythia8.h and uncomment pythia8 option


# Add include directory to compiler flags
CXXFLAGS += -I$(INC_DIR)
# in the converters section

# Define compiler
CXX = g++

# Define source directory
SRC_DIR := src
# Compile all *.cxx classes in the directory
SRC = $(shell find $(SRC_DIR) -name "*.cxx")
# Define include directory
INC_DIR := include
# Compile all *.h classes in the directory
HEADERS := $(shell find $(INC_DIR) -name "*.h" ! -name "*LinkDef*")
# Define directory with converters
CONV_DIR := converters

# Define flags
CXXFLAGS = $(shell root-config --cflags) -fPIC -W -Woverloaded-virtual -Wno-deprecated-declarations
CXXFLAGS += -Wall -pipe -std=c++17 -D__ROOT__ -I. -I$(INC_DIR)
LIBS = $(shell root-config --glibs) -lEG
INCS = $(shell root-config --incdir)

# Define output library	
MCDST = libMcDst.so


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
McDst_Dict.C: $(shell find $(INC_DIR) -name "*.h" ! -name "*LinkDef*")
	rootcint -f $@ -c -D__ROOT__ -I. -I$(INCS) $^ include/McDstLinkDef.h

.PHONY: clean distclean converters converters_debug converters_optdebug

clean:
	rm -vf src/*.o McDst_Dict*

distclean:
	rm -vf src/*.o McDst_Dict* $(MCDST) $(CONV_DIR)/urqmd2mc $(CONV_DIR)/pythia2mc $(CONV_DIR)/oscar2013ext2mc

converters_debug: CXXFLAGS += -O0 -g
converters_debug: converters
converters_optdebug: CXXFLAGS += -O2 -g
converters_optdebug: converters
converters: urqmd2mc  #pythia8
urqmd2mc: $(CONV_DIR)/urqmd2mc.cpp
	$(CXX) $(CXXFLAGS) -I$(INCS) $^ -o $(patsubst %.cpp,%,$<) -L. -l$(patsubst lib%.so,%,$(MCDST)) $(LIBS)
pythia2mc: $(CONV_DIR)/pythia8gen.cpp
	$(CXX) $(CXXFLAGS) -I$(INCS) $(shell pythia8-config --cflags) $^ -o $(patsubst %.cpp,%,$<) -L. -l$(patsubst lib%.so,%,$(MCDST)) $(shell pythia8-config --libs) $(LIBS)
oscar2013ext2mc: $(CONV_DIR)/oscar2013ext.cpp
	$(CXX) $(CXXFLAGS) -I$(INCS) $^ -o $(patsubst %.cpp,%,$<) -L. -l$(patsubst lib%.so,%,$(MCDST)) $(LIBS)
