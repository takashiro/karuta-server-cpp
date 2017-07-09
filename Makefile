
# Configurations #

TARGET = karuta-server
TARGETLIB = libkaruta.so

SOURCEDIR = \
	src/core \
	src/network \
	src/util \
	src

INCLUDEPATH = $(SOURCEDIR) include

SOURCES = src/main.cpp src/core/Application.cpp
LIBSOURCES = $(filter-out $(SOURCES), $(foreach dir,$(SOURCEDIR),$(wildcard $(dir)/*.cpp)))

CPPFLAGS = -std=c++11 -fexceptions -pthread -Wall
LFLAGS = -lpthread -ldl

BUILDCONFIG = release

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CPPFLAGS += -g
	BUILDCONFIG = debug
endif

BINDIR = bin/$(BUILDCONFIG)
OBJDIR = obj/$(BUILDCONFIG)
LIBDIR = lib/$(BUILDCONFIG)

EXECFLAGS = -L"$(BINDIR)" -lkaruta -Wl,-rpath='$$ORIGIN'

# Build Target #

OBJECTS = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
LIBOBJECTS = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(LIBSOURCES)))))

vpath %.cpp $(INCLUDEPATH)
vpath %.h $(INCLUDEPATH)

CCPARAM = $(CPPFLAGS) $(addprefix -I,$(INCLUDEPATH))

$(BINDIR)/$(TARGET): $(OBJECTS) | $(BINDIR)/$(TARGETLIB)
	$(CXX) -o $(BINDIR)/$(TARGET) $(OBJECTS) $(LFLAGS) $(EXECFLAGS)

$(BINDIR)/$(TARGETLIB): $(LIBOBJECTS) | $(BINDIR)
	$(CXX) -shared -o $(BINDIR)/$(TARGETLIB) $(LIBOBJECTS) $(LFLAGS)

.SECONDEXPANSION:
$(OBJECTS): $$(addsuffix .cpp,$$(basename $$(notdir $$@))) | $(OBJDIR)
	$(CXX) -o $@ -c $(filter %/$(addsuffix .cpp,$(basename $(notdir $@))),$(SOURCES)) $(CCPARAM)

$(LIBOBJECTS): $$(addsuffix .cpp,$$(basename $$(notdir $$@))) | $(OBJDIR)
	$(CXX) -o $@ -c $(filter %/$(addsuffix .cpp,$(basename $(notdir $@))),$(LIBSOURCES)) $(CCPARAM) -fPIC

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

$(BINDIR)/unittest: $(TESTOBJECTS) | $(BINDIR)/$(TARGETLIB)
	$(CXX) -o $(BINDIR)/unittest $(TESTOBJECTS) $(LFLAGS) $(EXECFLAGS)

$(TESTOBJECTS): $$(addprefix $(OBJDIR)/prep/unittest/, $$(addsuffix .cpp, $$(basename $$(notdir $$@)))) | $(OBJDIR)/unittest
	$(CXX) -o $@ -c $(addprefix $(OBJDIR)/prep/unittest/, $(addsuffix .cpp, $(basename $(notdir $@)))) $(CCPARAM) -Itest

$(TESTPREPS): $$(addprefix test/, $$(addsuffix .cpp, $$(basename $$(notdir $$@)))) $(OBJDIR)/prep/unittest
	./test/prep.sh test/$(notdir $@) > $@

$(OBJDIR)/unittest:
	mkdir -p $(OBJDIR)/unittest

$(OBJDIR)/prep/unittest:
	chmod +x test/prep.sh
	mkdir -p $(OBJDIR)/prep/unittest
