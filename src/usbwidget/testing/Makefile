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

# List of class definitions
CLASSDEPS=
CLASSDEPS=$(CLASSDEPS) Animal.hpp
CLASSDEPS=$(CLASSDEPS) Cat.hpp
CLASSDEPS=$(CLASSDEPS) StrayCat.hpp
CLASSDEPS=$(CLASSDEPS) TopCat.hpp

# List of classes to compile
CLASSOBJECTS=
CLASSOBJECTS=$(CLASSOBJECTS) Animal.obj
CLASSOBJECTS=$(CLASSOBJECTS) Cat.obj
CLASSOBJECTS=$(CLASSOBJECTS) StrayCat.obj
CLASSOBJECTS=$(CLASSOBJECTS) TopCat.obj

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
main.exe: $*.obj $(CLASSOBJECTS)

# Target for object-file for Executable
main.obj: $*.cpp $*.hpp $(CLASSDEPS)

# Targets for object-files for Classes
Animal.obj: $*.cpp $(CLASSDEPS)
Cat.obj: $*.cpp $(CLASSDEPS)
StrayCat.obj: $*.cpp $(CLASSDEPS)
TopCat.obj: $*.cpp $(CLASSDEPS)

# Run the program
run: main.exe
	@echo [$@]
	main.exe

# Cleanup intermediate files
clean:
	@echo [$@]
	rm -f -v *.obj *.exe
