
# Configurations #

TARGET = karuta-server

SOURCEDIR = \
	src/core \
	src/network \
	src/util \
	src

INCLUDEPATH = $(SOURCEDIR)

SOURCES = $(foreach dir,$(SOURCEDIR),$(wildcard $(dir)/*.cpp))

CPPFLAGS = -std=c++11 -fexceptions -pthread
LFLAGS = -lpthread

BINDIR = bin
OBJDIR = obj
BUILDCONFIG = release

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CPPFLAGS += -g
	BUILDCONFIG = debug
endif

# Build Target #

OBJECTS = $(addprefix $(OBJDIR)/$(BUILDCONFIG)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))

vpath %.cpp $(INCLUDEPATH)
vpath %.h $(INCLUDEPATH)

CCPARAM = $(CPPFLAGS) $(addprefix -I,$(INCLUDEPATH))

$(BINDIR)/$(BUILDCONFIG)/$(TARGET): $(OBJECTS) | $(BINDIR)/$(BUILDCONFIG)
	$(CXX) -o $(BINDIR)/$(BUILDCONFIG)/$(TARGET) $(wildcard $(OBJDIR)/$(BUILDCONFIG)/*.o) $(LFLAGS)

.SECONDEXPANSION:
$(OBJECTS): $$(addsuffix .cpp,$$(basename $$(notdir $$@))) | $(OBJDIR)/$(BUILDCONFIG)
	$(CXX) -o $@ -c $(filter %/$(addsuffix .cpp,$(basename $(notdir $@))),$(SOURCES)) $(CCPARAM)

$(BINDIR):
	mkdir -p $(BINDIR)

$(BINDIR)/$(BUILDCONFIG):
	mkdir -p $(BINDIR)/$(BUILDCONFIG)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/$(BUILDCONFIG):
	mkdir -p $(OBJDIR)/$(BUILDCONFIG)

clean:
	rm -rf $(OBJDIR)/$(BUILDCONFIG)
	rm -rf $(BINDIR)/$(BUILDCONFIG)

# Build Unit Test #

TESTUNITS = $(wildcard test/*.cpp)
TESTPREPS = $(addprefix $(OBJDIR)/prep/unittest/, $(addsuffix .cpp, $(basename $(notdir $(TESTUNITS)))))
TESTOBJECTS = $(addprefix $(OBJDIR)/$(BUILDCONFIG)/unittest/, $(addsuffix .o, $(basename $(notdir $(TESTUNITS)))))

test: $(BINDIR)/$(BUILDCONFIG)/unittest
	./$(BINDIR)/$(BUILDCONFIG)/unittest

$(BINDIR)/$(BUILDCONFIG)/unittest: $(OBJECTS) $(TESTOBJECTS) | $(BINDIR)/$(BUILDCONFIG)
	$(CXX) -o $(BINDIR)/$(BUILDCONFIG)/unittest $(filter-out %main.o, $(OBJECTS)) $(TESTOBJECTS) $(LFLAGS)

$(TESTOBJECTS): $$(addprefix $(OBJDIR)/prep/unittest/, $$(addsuffix .cpp, $$(basename $$(notdir $$@)))) | $(OBJDIR)/$(BUILDCONFIG)/unittest
	$(CXX) -o $@ -c $(addprefix $(OBJDIR)/prep/unittest/, $(addsuffix .cpp, $(basename $(notdir $@)))) $(CCPARAM) -Itest

$(TESTPREPS): $$(addprefix test/, $$(addsuffix .cpp, $$(basename $$(notdir $$@)))) $(OBJDIR)/prep/unittest
	./test/prep.sh test/$(notdir $@) > $@

$(OBJDIR)/$(BUILDCONFIG)/unittest:
	mkdir -p $(OBJDIR)/$(BUILDCONFIG)/unittest

$(OBJDIR)/prep/unittest:
	mkdir -p $(OBJDIR)/prep/unittest
