
# Configurations #

TARGET = karuta-server

SOURCEDIR = \
	src/core \
	src/network \
	src/util \
	src

INCLUDEPATH = $(SOURCEDIR)

SOURCES = $(foreach dir,$(SOURCEDIR),$(wildcard $(dir)/*.cpp))

CPPFLAGS = -std=c++11 -fexceptions -pthread -Wall
LFLAGS = -lpthread

BUILDCONFIG = release

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CPPFLAGS += -g
	BUILDCONFIG = debug
endif

BINDIR = bin/$(BUILDCONFIG)
OBJDIR = obj/$(BUILDCONFIG)
LIBDIR = lib/$(BUILDCONFIG)

# Build Target #

OBJECTS = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))

vpath %.cpp $(INCLUDEPATH)
vpath %.h $(INCLUDEPATH)

CCPARAM = $(CPPFLAGS) $(addprefix -I,$(INCLUDEPATH))

$(BINDIR)/$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CXX) -o $(BINDIR)/$(TARGET) $(wildcard $(OBJDIR)/*.o) $(LFLAGS)

.SECONDEXPANSION:
$(OBJECTS): $$(addsuffix .cpp,$$(basename $$(notdir $$@))) | $(OBJDIR)
	$(CXX) -o $@ -c $(filter %/$(addsuffix .cpp,$(basename $(notdir $@))),$(SOURCES)) $(CCPARAM)

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)

# Build Unit Test #

TESTUNITS = $(wildcard test/*.cpp)
TESTPREPS = $(addprefix $(OBJDIR)/prep/unittest/, $(addsuffix .cpp, $(basename $(notdir $(TESTUNITS)))))
TESTOBJECTS = $(addprefix $(OBJDIR)/unittest/, $(addsuffix .o, $(basename $(notdir $(TESTUNITS)))))

test: $(BINDIR)/unittest
	./$(BINDIR)/unittest

$(BINDIR)/unittest: $(OBJECTS) $(TESTOBJECTS) | $(BINDIR)
	$(CXX) -o $(BINDIR)/unittest $(filter-out %main.o, $(OBJECTS)) $(TESTOBJECTS) $(LFLAGS)

$(TESTOBJECTS): $$(addprefix $(OBJDIR)/prep/unittest/, $$(addsuffix .cpp, $$(basename $$(notdir $$@)))) | $(OBJDIR)/unittest
	$(CXX) -o $@ -c $(addprefix $(OBJDIR)/prep/unittest/, $(addsuffix .cpp, $(basename $(notdir $@)))) $(CCPARAM) -Itest

$(TESTPREPS): $$(addprefix test/, $$(addsuffix .cpp, $$(basename $$(notdir $$@)))) $(OBJDIR)/prep/unittest
	./test/prep.sh test/$(notdir $@) > $@

$(OBJDIR)/unittest:
	mkdir -p $(OBJDIR)/unittest

$(OBJDIR)/prep/unittest:
	mkdir -p $(OBJDIR)/prep/unittest
