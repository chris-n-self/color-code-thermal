
#
# MAKEFILE
# following hiltmon.com/blog/2013/07/03/a-simple-c-plus-plus-project-structure/
#

# Compiler
CC := g++

# Directories
SRCDIR := src
RUNDIR := run
BUILDDIR := build
TARGETDIR := bin

# Targets
EXECUTABLE := RunDecoderThresholdCalc
TARGET := $(TARGETDIR)/$(EXECUTABLE)

# Code Lists
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
SRCOBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/src/%,$(SOURCES:.$(SRCEXT)=.o))
RUNS := $(shell find $(RUNDIR) -type f -name *.$(SRCEXT))
RUNSOBJECTS := $(patsubst $(RUNDIR)/%,$(BUILDDIR)/runs/%,$(RUNS:.$(SRCEXT)=.o))

# Shared Compiler Flags
CFLAGS := -g -Wall -std=c++11
LIB := -L lib
INC := -I include

# generate executable
$(TARGET): $(SRCOBJECTS) $(RUNSOBJECTS)
	@mkdir -p $(TARGETDIR)
	@echo " Linking..."
	@echo " $(CC) $(SRCOBJECTS) $(BUILDDIR)/runs/$(EXECUTABLE).o -o $(TARGET) $(LIB)"; $(CC) $(SRCOBJECTS) $(BUILDDIR)/runs/$(EXECUTABLE).o -o $(TARGET) $(LIB)

# build sources
$(BUILDDIR)/src/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)/src
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

# build runmodes
$(BUILDDIR)/runs/%.o: $(RUNDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)/runs
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILDDIR) $(TARGETDIR)"; $(RM) -r $(BUILDDIR) $(TARGETDIR)

.PHONY: clean
