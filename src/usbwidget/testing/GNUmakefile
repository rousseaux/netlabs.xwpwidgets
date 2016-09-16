###############################################################################
# GNUmakefile :: Build the test program                                       #
# --------------------------------------------------------------------------- #
# This GNUmakefile is for use with GNU Make.                                  #
# (Using Linux for now)                                                       #
###############################################################################

# -----------------------------------------------------------------------------
# Note: Mimic NMake instead of being GNU Make optimized
# -----------------------------------------------------------------------------
# The UsbWidget is built using NMake.
# However, this testing module also builds under Linux with GNU Make and GCC.
# To be able to easily replicate changes, this makefile mimics NMake and does
# not use GNU Make capabilities beyond those needed. The .SECONDEXPANSION
# directive is used to mimic NMakes ability to process $* in dependency lists.

# Compiler and Linker macros
CC:=g++
CCFLAGS:=-D__FUNCTION__=__PRETTY_FUNCTION__
LD:=g++
LDFLAGS:=

# List of class definitions
CLASSDEPS:=
CLASSDEPS:=$(CLASSDEPS) Animal.hpp
CLASSDEPS:=$(CLASSDEPS) Cat.hpp
CLASSDEPS:=$(CLASSDEPS) StrayCat.hpp
CLASSDEPS:=$(CLASSDEPS) TopCat.hpp

# List of classes to compile
CLASSOBJECTS:=
CLASSOBJECTS:=$(CLASSOBJECTS) Animal.obj
CLASSOBJECTS:=$(CLASSOBJECTS) Cat.obj
CLASSOBJECTS:=$(CLASSOBJECTS) StrayCat.obj
CLASSOBJECTS:=$(CLASSOBJECTS) TopCat.obj

# Required for inference rules to function correctly
.SUFFIXES:
.SUFFIXES: .cpp .obj .elf

# Enable the use of $$@, $$* and target-specific variables in dependency lists
.SECONDEXPANSION:

# Rule to build object-files from cpp-files
.cpp.obj:
	$(info )
	@echo [$@]
	@echo Reason: $?
	$(CC) -c $(CCFLAGS) $< -o $@

# Rule to build an executable from object-files
.obj.elf:
	$(info )
	@echo [$@]
	@echo Reason: $?
	$(LD) $(LDFLAGS) $^ -o $@

# Build the whole shebang
all: main.elf;

# Target for the Executable
main.elf: $(CLASSOBJECTS)

# Target for object-file for Executable
main.obj: $$*.hpp $(CLASSDEPS)

# Targets for object-files for Classes
Animal.obj: $$*.cpp $(CLASSDEPS)
Cat.obj: $$*.cpp $(CLASSDEPS)
StrayCat.obj: $$*.cpp $(CLASSDEPS)
TopCat.obj: $$*.cpp $(CLASSDEPS)

# Run the program
run: main.elf
	$(info )
	@echo [$@]
	./main.elf

# Cleanup intermediate files
clean:
	$(info )
	@echo [$@]
	rm -f -v *.obj *.elf
