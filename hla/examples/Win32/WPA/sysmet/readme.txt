There are three RadASM projects in this directory
and several batch files to provide different options
for RadASM and command-line users.

"Windows Programming in Assembly" assumes that you're
using a make program (typically Borland's make.exe
software) and the "standard" RadASM project file is
set up for using Borland's make.

Some users have Microsoft's nmake.exe program.
Some users don't have a version of make at all
(although you can get a free copy from Borland's
web site, see "Windows Programming in Assembly",
Chapter Four for details).

The extra RadASM project files found in this
directory and the batch files are for those
users who prefer not to use Borland's make.exe
program.


------------------------------------------------
Standard Build via make.exe: sysmet.rap

This version assumes that you've installed Borland's
make.exe (or some equivalent program named make.exe)
in your execution path (e.g., the HLA subdirectory).

This project file uses the "makefile" file to control
the assembly options found in the RadASM "MAKE" menu.

------------------------------------------------
Build via nmake.exe: sysmet_nmake.rap

This version assumes that you've installed Microsoft's
nmake.exe in your execution path (e.g., the HLA 
subdirectory).

This project file uses the "makefile" file to control
the assembly options found in the RadASM "MAKE" menu.

------------------------------------------------
Batch Files: sysmet_bat.rap

The Lines_bat.rap file invokes batch files
to do the compilation rather than make or nmake.
The RadASM make menu items do the following

BUILD- invokes build.bat
BUILD ALL - invokes build.bat
COMPILE RC - invokes compilerc.bat
SYNTAX - invokes syntax.bat
RUN - invokes run.bat

Note that these batch files always recompile
the application, even if the current object
files and executables are up to date (i.e.,
you don't get the benefits of using the make
utility here). Not that it really matters
for such a small program.

You can also invoke these batch files directly from
a command window shell prompt if you don't have
a make utility available.

------------------------------------------------