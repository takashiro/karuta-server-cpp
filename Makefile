
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

DEBUGOBJECTS = $(addprefix $(OBJDIR)/debug/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
RELEASEOBJECTS = $(addprefix $(OBJDIR)/release/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))

vpath %.cpp $(INCLUDEPATH)
vpath %.h $(INCLUDEPATH)

CCPARAM = $(CPPFLAGS) $(addprefix -I,$(INCLUDEPATH))

$(BINDIR)/$(TARGET): $(RELEASEOBJECTS) | $(BINDIR)
	$(CXX) -o $(BINDIR)/$(TARGET) $(wildcard $(OBJDIR)/release/*.o) $(LFLAGS)

.SECONDEXPANSION:
$(DEBUGOBJECTS): $$(addsuffix .cpp,$$(basename $$(notdir $$@))) | $(OBJDIR)/debug
	$(CXX) -o $@ -c $(filter %/$(addsuffix .cpp,$(basename $(notdir $@))),$(SOURCES)) $(CCPARAM) -g

$(RELEASEOBJECTS): $$(addsuffix .cpp,$$(basename $$(notdir $$@))) | $(OBJDIR)/release
	$(CXX) -o $@ -c $(filter %/$(addsuffix .cpp,$(basename $(notdir $@))),$(SOURCES)) $(CCPARAM) -g

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/debug:
	mkdir -p $(OBJDIR)/debug

$(OBJDIR)/release:
	mkdir -p $(OBJDIR)/release

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)

# Build Unit Test #

TESTUNITS = $(wildcard test/*.cpp)
TESTPREPS = $(addprefix $(OBJDIR)/unittest/prep/, $(addsuffix .cpp, $(basename $(notdir $(TESTUNITS)))))
TESTOBJECTS = $(addprefix $(OBJDIR)/unittest/, $(addsuffix .o, $(basename $(notdir $(TESTUNITS)))))

test: $(BINDIR)/unittest
	./$(BINDIR)/unittest

$(BINDIR)/unittest: $(DEBUGOBJECTS) $(TESTOBJECTS) | $(BINDIR)
	$(CXX) -o $(BINDIR)/unittest $(filter-out %main.o, $(DEBUGOBJECTS)) $(TESTOBJECTS) $(LFLAGS)

$(TESTOBJECTS): $$(addprefix $(OBJDIR)/unittest/prep/, $$(addsuffix .cpp, $$(basename $$(notdir $$@))))
	$(CXX) -o $@ -c $(addprefix $(OBJDIR)/unittest/prep/, $(addsuffix .cpp, $(basename $(notdir $@)))) $(CCPARAM) -g -Itest

$(TESTPREPS): $$(addprefix test/, $$(addsuffix .cpp, $$(basename $$(notdir $$@)))) $(OBJDIR)/unittest/prep
	./test/prep.sh test/$(notdir $@) > $@

$(OBJDIR)/unittest:
	mkdir -p $(OBJDIR)/unittest

$(OBJDIR)/unittest/prep:
	mkdir -p $(OBJDIR)/unittest/prep
