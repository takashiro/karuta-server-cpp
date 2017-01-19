
# Configurations #

TARGET = karuta-server

SOURCEDIR = \
	src/core \
	src/network \
	src/util \
	src

INCLUDEPATH = $(SOURCEDIR)

SOURCES = $(foreach dir,$(SOURCEDIR),$(wildcard $(dir)/*.cpp))

CPPFLAGS = -std=c++11 -fexceptions
LFLAGS =

BINDIR = bin
OBJDIR = obj

# Build Target #

OBJECTS = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))

vpath %.cpp $(INCLUDEPATH)
vpath %.h $(INCLUDEPATH)

CCPARAM = $(CPPFLAGS) $(addprefix -I,$(INCLUDEPATH))

$(BINDIR)/$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CXX) -o $(BINDIR)/$(TARGET) $(wildcard $(OBJDIR)/*.o) $(LFLAGS)

$(OBJECTS): $(addsuffix .cpp,$(basename $(notdir $@))) | $(OBJDIR)
	$(CXX) -o $@ -c $(filter %/$(addsuffix .cpp,$(basename $(notdir $@))),$(SOURCES)) $(CCPARAM)

$(BINDIR):
	mkdir $(BINDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)

# Build Unit Test #

TESTUNITS = $(wildcard test/*.cpp)

TESTOBJECTS = $(addprefix $(OBJDIR)/unittest/, $(addsuffix .o, $(basename $(notdir $(TESTUNITS)))))

test: $(OBJECTS) $(TESTOBJECTS) | $(BINDIR)
	$(CXX) -o $(BINDIR)/unittest $(filter-out %main.o, $(wildcard $(OBJDIR)/*.o)) $(wildcard $(OBJDIR)/unittest/*.o) $(LFLAGS)
	./bin/unittest

$(TESTOBJECTS): $(addprefix test/, $(addsuffix .cpp, $(basename $(notdir $@)))) | $(OBJDIR)/unittest
	$(CXX) -o $@ -c $(addprefix test/, $(addsuffix .cpp, $(basename $(notdir $@)))) $(CCPARAM) -Itest

$(OBJDIR)/unittest:
	mkdir $(OBJDIR)/unittest
