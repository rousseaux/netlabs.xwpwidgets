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

# Class Modules to build
CLASS_MODULES:=
CLASS_MODULES:=$(CLASS_MODULES) Animal@
CLASS_MODULES:=$(CLASS_MODULES) Cat@
CLASS_MODULES:=$(CLASS_MODULES) StrayCat@
CLASS_MODULES:=$(CLASS_MODULES) TopCat@

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
main.elf: $(CLASS_MODULES:@=.obj)

# Target for object-file for Executable
main.obj: $$*.hpp $(CLASS_MODULES:@=.obj)

# Targets for object-files for Classes
Animal.obj: $$*.cpp $(CLASS_MODULES:@=.hpp)
Cat.obj: $$*.cpp $(CLASS_MODULES:@=.hpp)
StrayCat.obj: $$*.cpp $(CLASS_MODULES:@=.hpp)
TopCat.obj: $$*.cpp $(CLASS_MODULES:@=.hpp)

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
