# Copyright (c) 1998 Compuware Corporation
#
# Makefile for HELLO example enhanced to build into separate directories
#	for Debug and Release builds, and show off some advanced makefile
#	tricks and techniques.

# This is the makefile for simple VxD HELLO

# Define the name of the VxD
DEVICENAME = HELLO

# Specify that the VxD uses the C++ framework
FRAMEWORK = CPP

# Specify that the VxD supports dynamic loading
DYNAMIC = 1

# By default, build for debugging so that the debug message is printed.
# Note that the DEBUG macro defined here can be overridden by a DEBUG
# macro passed in on the command line (which has precedence).  Thus either
# a debug or release build can be built without modifying the makefile
# by defining a DEBUG macro on the command line when NMAKE is invoked.
DEBUG = 1

# The macro BROWSE controls whether or not browse information is built for
# the project.  To define the macro and build browse information uncomment
# the next line
BROWSE	= 1

# The macro MAKE_RETAIL_SYMBOLS controls whether or not you generate debug
# symbols for your retail build.  This can be helpful for debugging the
# retail build.  To define the macro and create the debugging symbols,
# uncomment the next line.
#MAKE_RETAIL_SYMBOLS = 1

# The next line defines the XFLAGS macro flags that is available to specify
# extra flags that will be used by the compiler during compilation.
# As an example we will generate assembler output mixed with the source
# in .cod files.  Generally you would only do this when you are debugging.
# The /FAcs flag turns on the .cod file generation and the /Fa flag specifies
# the destination directory for the generated file.  Note that we use the 
# OBJPATH macro in the directory path. This macro is used to define a path
# for the object and intermediary files.  You can define this macro yourself,
# or you can choose to use the MAKE_DEBUG_RELEASE_DIRS macro (as shown below),
# which internally defines the path depending on what type of build you are
# doing (debug or release).  Note that you cannot both specify OBJPATH and use
# the MAKE_DEBUG_RELEASE_DIRS macro.
XFLAGS = /FAcs /Fa$(OBJPATH)^\

# Specify any libraries that you need to link with.
# (Note, this simple example has no libraries)
USER_LIB	=

# You can also specify paths for include files (other than local).  The
# INCVXD2 macro is used by the VToolsD makefiles for this purpose
INCVXD2		= .\Alt_Inc

# You can also specify paths for include files (other than local).  The
# SOURCEPATH macro is used by the VToolsD makefiles for this purpose
SOURCEPATH	= .\Alt_Src

# We need to ultimately provide a list of OBJECTS that the VToolsD makefile
# will use to build our VxD.  There are two ways to do this.  We can define
# the OBJECTS macro here with all of the object names.  Alternately, we
# can use the SOURCES macro, which allows us to specify the source files in
# any combination of .c .cpp and .asm files.  This will be converted into
# the OBJECTS macro by the vtoolsd.mak makefile.  Note that if you use the
# SOURCES macro, you must not specify the OBJECTS macro (it will be expanded
# internally by the VToolsD makefiles).  Note also that the SOURCES macro is
# not available when using Borland build tools.
SOURCES		=	 \
			hello.cpp	\
			althello.cpp	

# Defining the MAKE_DEBUG_RELEASE_DIRS macro causes VToolsD to automatically
# create a .\Debug directory for a debug build, or a .\Release directory for
# a non-debug build.  All of the intermediary files are placed in the created
# directory, along with the output files if OUTPATH is not defined (see below).
# Dependency checking is done correctly so incremental builds only build out
# of date files.
#
# IMPORTANT: Use of this macro precludes including .lib files in the list
# of OBJECTS (or SOURCES).  Use USER_LIB macro instead to specify .lib files
# to link with.  To define the macro and uncomment the next line.
MAKE_DEBUG_RELEASE_DIRS = 1

# The OUTPATH macro specifies a path to which the output files created by
# the build will be sent.  This includes .vxd .exp .lib .map files as well
# as .nms and .bsc if created.  Note, if not defined, by default, this macro
# will be defined to be the same as OBJDIR (if defined, otherwise all files
# will end up in the directory with the makefile).
#OUTPATH = .\Output

# Since nmake will build the first target it finds if a specific target is
# not requested, placing a target before including the VToolsD makefiles allows
# you to do some fancy things with psuedotargets.  Just make sure to specify
# $(TARGNAME) as a dependency of this target.  Since this macro isn't defined
# until after inclusion of the VToolsD makefiles, make a psuedotarget for it as
# well.  We show this technique by defining a target "default", with several
# pseudotarget dependencies.
MY_DIR = .\MyDir
default: build_target $(MY_DIR) copy_target echo_objects

# Include the necessary files from VToolsD
!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

# Pseudotarget to allow us to build the actual VxD, included after the VToolsD
# makefiles
build_target: $(TARGNAME)

# Define the dependencies for the object modules
COMMON_INCLUDE = .\Alt_Inc
hello.cpp:						hello.h $(COMMON_INCLUDE)\althello.h
$(SOURCEPATH)\althello.cpp:		$(COMMON_INCLUDE)\althello.h

# A pseudotarget such as copy_target allows more control than OUTPATH for
# copying a file to a desired location.
copy_target: $(TARGNAME)
! if "$(DEBUG)" == "1"
copy_target: $(OUTPATH)\$(DEVICENAME).nms
! endif
	@echo Copying files.
	!@copy $** $(MY_DIR)

# Pseudotarget to create a directory
$(MY_DIR):
	@echo Checking for existence of my dir : $(MY_DIR)
	@if not exist $(MY_DIR)\NUL mkdir $(MY_DIR)

# Pseudotarget to echo the objects.  FINALOBJECTS is a macro use internally
# by the VToolsD makefiles to process the objects throught the compiler.
# It is used here for demonstration purposes only to show what the final
# objects look like.  Note that in this example we have used the SOURCES
# macro to specify our files and the MAKE_DEBUG_RELEASE_DIRS macro to augment
# the pathnames so we end up building into a different directory depending on
# build type (debug or release).
echo_objects:
	@echo FinalObjects $(FINALOBJECTS)

