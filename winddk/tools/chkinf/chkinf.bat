@rem = '--*-PERL-*--';
@rem = '
@echo off
setlocal

set BatFilePath=%~dp0
if "%BatFilePath:~-1,1%"=="\" set BatFilePath=%BatFilePath:~0,-1%

set ARGS=
:loop
if "%~1" == "" goto endloop
set ARGS=%ARGS% %1
shift
goto loop
:endloop
rem ***** This assumes PERL is in the PATH *****

rem ** If we are on Windows 2000, we can search the path to force inclusion
rem ** of the proper modules so long as this is in our path using this bit
rem ** of code - fails on all other OSes, so test for 2000 specific variable.
rem ** Append path of this script to PATH (to allow for invocation from other
rem ** directories without having to explicitly set PATH beforehand.
PATH=%PATH%;%BatFilePath%
set PLibPath=
if not "%ALLUSERSPROFILE%"=="" (
   for %%I in (%PATH%) do (
      if exist %%~fsI\chkinf.bat (
         set PLibPath=-I%%I
         goto runit
      )
   )
)
:runit

perl.exe %PerlFlags% %PLibPath% "%BatFilePath%\chkinf.bat" %ARGS%

set ARGS=
goto endofperl
@rem ';
# Copyright 1999-2001 Microsoft Corporation

#system("cls"); # Clear the screen so we can reduce clutter.
###################################################################################################
# +---------------------------------------------------------------------------------------------+ #
# | Dec 18, 1998  Created                                                                       | #
# |                                                                                             | #
# | Usage :  ChkInf file1.inf [file2.inf] [file3.inf] ...                                       | #
# | Output:  .\htm\file1.htm                                                                    | #
# |          ....                                                                               | #
# |          .\htm\summary.htm                                                                  | #
# |                                                                                             | #
# | SYNOPSIS: Check INF files and generate an .html report                                      | #
# +---------------------------------------------------------------------------------------------+ #
###################################################################################################
#----------------------------------------------------------------------#
# Modules to Load                                                      #
#----------------------------------------------------------------------#
use strict;    # Force clean code
use SmartGlob; # File globbing
use ChkInf;    # ChkInf specific module
#----------------------------------------------------------------------#
# Date & Version: Update these if you make changes to this file!       #
#----------------------------------------------------------------------#
my($Date)             = "03/14/2001";      # Date of version
my($Version)          = ChkInf::Version(); # Version of this file
#----------------------------------------------------------------------#
#  Utility Subroutines                                                 #
#----------------------------------------------------------------------#
sub TRUE  {return(1);} # For BOOLEAN testing
sub FALSE {return(0);} # For BOOLEAN testing
#----------------------------------------------------------------------#
# Variables for Storing CommandLine Options                            #
#----------------------------------------------------------------------#
my @InfList;           # List of INF's we're going to check
my $Browser;           # Invoke Web Browser?
my($LOG, $LogFile);    # Name of NT Log File
my $LayoutCheck =0;    # Process against layout.inf
my $DeviceClassOpt=""; # Options to pass to Device Specific Module
my $ExitCode   = 0;    # Exit Code to set %ERRORLEVEL%
my $DevInfOnly = FALSE;# If TRUE, don't write non-dev inf summaries
my $Internal   = FALSE;# If TRUE, some checks change

#----------------------------------------------------------------------#
# Global Variables to Track Per Invocation                             #
#----------------------------------------------------------------------#
my($NumFiles)         =   0;     # Count of total files processed.
my($Overwrite)        =   "Y";   # Prompt to overwrite = by default
#----------------------------------------------------------------------#
# Global Constants                                                     #
#----------------------------------------------------------------------#
my($HTM)              = ".\\htm";# Subdirectory for output
#----------------------------------------------------------------------#
# Print Copyright Information                                          #
#----------------------------------------------------------------------#
print(STDERR "\nChkInf: Copyright 1998-2001 Microsoft Corporation\n");
print(STDERR "        Version: $Version   Created: $Date\n\n");
#----------------------------------------------------------------------#
# Parse command-line arguments & PreProcess                            #
#----------------------------------------------------------------------#
ParseArgs: {
    # If there are no parameters, just show usage & quit.
    if ($#ARGV == -1 ) {
        Usage(),
        die("\n");
    }


    my($lpCount); # Need this just for looping.
    for ($lpCount=0; $lpCount <= ($#ARGV); $lpCount++) {
    SWITCH: {
        Usage()                 , die("\n")                           if uc(substr($ARGV[$lpCount],0,2)) eq "/?";
        Usage()                 , die("\n")                           if uc(substr($ARGV[$lpCount],0,2)) eq "-?";
        $Browser                = TRUE                   ,last SWITCH if uc(substr($ARGV[$lpCount],0,2)) eq "/B";
        $DevInfOnly             = TRUE                   ,last SWITCH if uc(substr($ARGV[$lpCount],0,3)) eq "/DO";
        $LayoutCheck            = TRUE                   ,last SWITCH if uc(substr($ARGV[$lpCount],0,3)) eq "/LO";
        $Internal               = TRUE                   ,last SWITCH if uc(substr($ARGV[$lpCount],0,3)) eq "/MS";
        $Overwrite              = "A\n"                  ,last SWITCH if uc(substr($ARGV[$lpCount],0,2)) eq "/A";
        $LogFile                = $ARGV[++$lpCount]      ,last SWITCH if uc(substr($ARGV[$lpCount],0,2)) eq "/L";
        $DeviceClassOpt         = join("&", ($DeviceClassOpt,"$ARGV[++$lpCount]"))
                                                         ,last SWITCH if uc(substr($ARGV[$lpCount],0,3)) eq "/DC";
        $InfList[($#InfList+1)] = "$ARGV[$lpCount]"      ,last SWITCH;# If we didn't get a flag, must be a dir or file
        1; #Last line should do nothing
        }
    }

    if ($#InfList < 0 ) {
        print(STDERR "ERROR: No input files specified!\n\n");
        Usage(),
        die("\n");
    }

    # Expand wildcards & directories in InfList
    @InfList = SmartGlob::SmartGlob(@InfList);

    # Create .\htm & see if summary log exists.
    if (! mkdir("htm",0777)) {
        if (-e ".\\htm\\summary.htm") {
            print(STDERR "\nSummary file .\\htm\\summary.htm already exists. Overwrite (Yes/No/All) [Y] ? ")
                    unless ($Overwrite =~ /^A/);
            $Overwrite = <STDIN> unless ($Overwrite =~ /^A/);
            chop($Overwrite);
            if ($Overwrite =~ /^[nN]/) {die("Save contents of .\\htm and restart.\n");}
        }
    }

    # Open the Logfile if requested
    if (defined($LogFile)) {
        if ((-e "$LogFile") && ($Overwrite !~ /^[aA]/)) {
            printf(STDERR "Log file $LogFile already exists. Overwrite (Yes/No/All) [Y] ? ",uc($Overwrite));
            $Overwrite = <STDIN>;
            chop($Overwrite);
            if ($Overwrite =~ /^[nN]/) {die("Save contents of $LogFile and restart.\n");}
        }
        if (! open(LOG, ">>$LogFile")) {
            print(STDERR  "Error opening log file $LogFile: $!\n");
            print(STDERR  "  Continuing without creating log.\n");
            undef($LogFile);
        } else {
            ChkInf::CreateLog($LogFile);
            close(LOG);
        }
    }
    undef($lpCount); # Free up the Memory.
 }

###################################################################################################
# +---------------------------------------------------------------------------------------------+ #
# |                                                                                             | #
# |      Main Body of Program (i.e. Where the work gets done)                                   | #
# |                                                                                             | #
# +---------------------------------------------------------------------------------------------+ #
###################################################################################################
Main: {
    # Variables to be reset for each INF
    my($InfFile);    # Name of this INF
    my($OutFile);    # Name of HTM we'll be writing

    # Begin a timer... useful for monitoring performance.
    ChkInf::BeginTimer("MAIN");

    if ($LayoutCheck) {
        no strict;  # Turn off strict for a moment
         # Using undeclared var
        my($LOFile) = $ENV{"windir"}."\\inf\\layout.inf";
        use strict; # Back on

        ChkInf::GetLayout($LOFile);
    }

    foreach $InfFile (@InfList) {

        if (-s $InfFile == 0) {
            print(STDERR "Skipping $InfFile: File contains no data.\n");
            next;
        }

        ChkInf::ResetGlobals($InfFile,$DeviceClassOpt, $Internal); # Init per-INF globals in ChkInf.pm

        $OutFile     = '';

        # Abort unless we can open the INF
        unless (open(INF, "$InfFile")) {
            print(STDERR "ERROR: Cannot open $InfFile. Skipping it.\n");
            next;
        }
        close(INF); # Because our 'unless' opened it.

        # Need to extract just the filename from a string that may include a directory path
        if ($InfFile =~ m!^(.*)\\(.*)\.inf!i) {  # Handles Path + Filename
            # include munged form of path (:, ., and \ replaced with +) in the .HTM name to avoid
            #   filename collisions when multiple INFs are processed (@InfList > 1 entry)
            my $temp = $1;
            $temp =~ s/\+/\+\+/;  # replace '+' with '++'
            $temp =~ tr/\:\\\./\+\+\+/;  # replace ':', '\\', and '.' with '+'
            $OutFile = ".\\htm\\" . $temp . "__" . $2 . ".htm";
        } elsif ($InfFile =~ m!^(.*)\.inf!i) { # Handles Filename w/o Path
            $OutFile = ".\\htm\\" . $1 . ".htm";
        } else {
            # If we get here things are pretty bad, it means $InfFile does not
            #   end in '.inf' and SmartGlob::SmartGlob is messed up!
            print(STDERR "ERROR: Could not parse filename $InfFile correctly!\n");
            next;
        }

        # If a report exists already, verify we should overwrite it.
        if ((-e "$OutFile") && ($Overwrite !~ /^[Aa]/)) {
            print(STDERR "\nOutput file $OutFile already exists. Overwrite (Yes/No/All) [Y] ? ");
            $Overwrite = <STDIN>;
            chop($Overwrite);
            if ($Overwrite =~ /^[nN]/) {print(STDERR "$InfFile skipped\n"); next;}
        }

        #--------------------------------------------------------#
        # If we made it through all of that, we're finally going #
        # to process the INF...                                  #
        #--------------------------------------------------------#

        # Increment out file count & let user know we're proceeding with this file
        $NumFiles++;
        print(STDERR "\nProcessing $InfFile -> $OutFile \n");

        # If the INF is a device INF or all results are wanted
        #   print the summary.
        if (ChkInf::ProcInf($InfFile) or (! $DevInfOnly)) {
            # Spit out the results of this INF
            ChkInf::PrintResults($InfFile, $OutFile);
        }
    }
    #--------------------------------------------------------#
    # All INFs are processed                                 #
    #--------------------------------------------------------#
    # End the timer (which reports total runtime)
    ChkInf::EndTimer("MAIN");

    # Print summary of all INFs checked
    $ExitCode = ChkInf::PrintSummary("htm\\summary.htm");

    # Invoke the browser if requested!
    if ($Browser) {
        system("start htm\\summary.htm");
    }
    exit($ExitCode);
}
# END OF EXECUTABLE

#-------------------------------------------------------------------------------------------------#
# Displays help information on invocation of ChkInf                                               #
#-- Usage() --------------------------------------------------------------------------------------#
sub Usage {
    print(STDERR "Usage: ChkInf[.bat] <FileList> [/L <LogFile>] [/B] [/DC <Options>] [/DO]\n");
    print(STDERR "        <FileList>    = FileName | Wildcard List | DirectoryName\n");
    print(STDERR "        /L  <LogFile> = Write an NT Log File Named <LogFile>\n");
    print(STDERR "        /B            = Launch Default Browser & Display Results\n");
    print(STDERR "        /DO           = Don't write summaries for non-device INFs\n");
    print(STDERR "        /DC <Options> = Use Device Class <Options> When Checking INFs\n");
    print(STDERR "        /LO           = Verify CopyFiles against Layout.inf\n");
} # Usage()
__END__
:endofperl
endlocal

