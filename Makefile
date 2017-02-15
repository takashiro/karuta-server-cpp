
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

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)

# Build Unit Test #

TESTUNITS = $(wildcard test/*.cpp)
TESTPREPS = $(addprefix $(OBJDIR)/unittest/prep/, $(addsuffix .cpp, $(basename $(notdir $(TESTUNITS)))))
TESTOBJECTS = $(addprefix $(OBJDIR)/unittest/, $(addsuffix .o, $(basename $(notdir $(TESTUNITS)))))

test: $(BINDIR)/unittest
	./$(BINDIR)/unittest

$(BINDIR)/unittest: $(OBJECTS) $(TESTOBJECTS) | $(BINDIR)
	$(CXX) -o $(BINDIR)/unittest $(filter-out %main.o, $(wildcard $(OBJDIR)/*.o)) $(wildcard $(OBJDIR)/unittest/*.o) $(LFLAGS)

$(TESTOBJECTS): $$(addprefix $(OBJDIR)/unittest/prep/, $$(addsuffix .cpp, $$(basename $$(notdir $$@))))
	$(CXX) -o $@ -c $(addprefix $(OBJDIR)/unittest/prep/, $(addsuffix .cpp, $(basename $(notdir $@)))) $(CCPARAM) -Itest

$(TESTPREPS): $$(addprefix test/, $$(addsuffix .cpp, $$(basename $$(notdir $$@)))) $(OBJDIR)/unittest/prep
	./test/prep.sh test/$(notdir $@) > $@

$(OBJDIR)/unittest:
	mkdir -p $(OBJDIR)/unittest

$(OBJDIR)/unittest/prep:
	mkdir -p $(OBJDIR)/unittest/prep
