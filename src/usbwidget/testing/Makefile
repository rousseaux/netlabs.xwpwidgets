###############################################################################
# Makefile :: Build the test program                                          #
# --------------------------------------------------------------------------- #
# This Makefile is for use with NMake from IBM Visual Age v3.0.               #
###############################################################################

# -----------------------------------------------------------------------------
# Quirk: Inferred dependency _last_ in list when rule invoked
# -----------------------------------------------------------------------------
# NMake v3.x _appends_ the inferred dependency to the list when a rule is
# invoked. So, it is _not_ the first dependency which makes $< less usable and
# also influences the name of the target when it is derived from the first
# object. To solve this, the $* macro is used explicitly in dependency lists
# before declaring other dependencies, so it is always _first_ in the list.

# Compiler and Linker macros
CC=icc
CCFLAGS=-Q+
LD=icc
LDFLAGS=-Q+

# Class Modules to build
CLASS_MODULES=\
Animal@\
Cat@\
StrayCat@\
TopCat@\

# Required for inference rules to function correctly
.SUFFIXES:
.SUFFIXES: .cpp .obj .exe

# Rule to build object-files from cpp-files
.cpp.obj:
	@echo.
	@echo [$@]
	@echo Reason: $?
	$(CC) -c $(CCFLAGS) $*.cpp -Fo$@

# Rule to build an executable from object-files
.obj.exe:
	@echo.
	@echo [$@]
	@echo Reason: $?
	$(LD) $(LDFLAGS) $** -Fo$@

# Build the whole shebang
all: main.exe

# Target for the Executable
main.exe: $*.obj $(CLASS_MODULES:@=.obj)

# Target for object-file for Executable
main.obj: $*.cpp $*.hpp $(CLASS_MODULES:@=.obj)

# Targets for object-files for Classes
Animal.obj: $*.cpp $(CLASS_MODULES:@=.hpp)
Cat.obj: $*.cpp $(CLASS_MODULES:@=.hpp)
StrayCat.obj: $*.cpp $(CLASS_MODULES:@=.hpp)
TopCat.obj: $*.cpp $(CLASS_MODULES:@=.hpp)

# Run the program
run: main.exe
	@echo [$@]
	main.exe

# Cleanup intermediate files
clean:
	@echo [$@]
	rm -f -v *.obj *.exe
