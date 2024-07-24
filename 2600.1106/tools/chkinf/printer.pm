# (C) Copyright 1999-2001 Microsoft Corporation

# The name of this module. For our purposes it must be all in caps.
package PRINTER;

    use strict;                # Keep the code clean
    #---------------------------------------------------#
    # Module level variables
    #---------------------------------------------------#
    my($Version) = "5.1.2505.0";    # Version of this file
    my($Current_INF_File);          # Stores name of the current INF file.
    my(@Device_Class_Options);      # Array to store device specific options.

    my $DEBUG = 0;                  # 0, 1, 2, or 3
    my %AllModels;
    my @NTCpuDecorations      = (".NTx86", ".NTIA64", ".AMD64");
    my @SectionDecorations    = (@NTCpuDecorations, ".NT");   # must be in priority order!
    my @SectionAllDecorations = (@SectionDecorations, "");    # must be in priority order!

    my $NTPrintINF_File;            # handle to system's NTPrint.inf file - used to verify uniqueness
    my $NTPrintINF_path       = "$ENV{SystemRoot}\\inf\\ntprint.inf";
    my %NTPrintINFModelNames;       # Assoc array of all NTPrint.inf model names
    my @NTPrintSDFiles;             # Cached list of files in NTPrint.inf SourceDisksFiles sections
    my $NTPrintINF_Included;        # Boolean - Is NTPrint.INF included by the current INF

    my $LayoutINF_File;             # handle to system's Layout.inf file - used to verify uniqueness
    my $LayoutINF_path        = "$ENV{SystemRoot}\\inf\\layout.inf";
    my @LayoutSDFiles;              # Cached list of files in Layout.inf SourceDisksFiles sections
    my $LayoutINF_Included;         # Boolean - Is Layout.INF included by the current INF

    my @SDFiles;                    # Cached list of files in original INF's SourceDisksFiles sections
    my @InteractiveInstallPnpIDs;   # List of HWid's in the InteractiveInstall directive of ControlFlags section
    my %CurrentINFModelHWids;       # Assoc array of all current inf model HWids
    my %CurrentINFModelNames;       # Assoc array of all current inf model names and their install section names
    my %CurrentINFManufacturerNames; # Assoc array of all current inf manufacturer names (typically only one)

    my @PnpBusPrefixes = ( # don't include trailing backslash
                          "LPTENUM",
                          "USBPRINT", 
                          "DOT4PRT",
                          "MF",
                         );

    my @NoReferencesAllowed = (
                     "UNIDRV.DLL",
                     "UNIDRVUI.DLL",
                     "UNIRES.DLL",
                     "UNIDRV.HLP",
                     "STDNAMES.GPD",
                     "TTFSUB.GPD",
                     "PJLMON.DLL",
                     "sRGB.ICM",
                     "LOCALE.GPD",
                   #  "GPDPARSE.DLL",  # old file ????
                     );
                     
    my @RedistributableFiles = (
                     "PSCRIPT5.DLL",
                     "PS5UI.DLL",
                     "PSCRIPT.NTF",
                     "PSCRIPT.HLP",
                     "PSCRPTFE.NTF",
                     );

    my @AllowReferences = (
       "APDLRES.DLL",  "ATEPJRES.DLL", "BR24RES.DLL",  "BR9RES.DLL",   "BRHJRES.DLL",
       "BUL18RES.DLL", "BUL24RES.DLL", "BULL9RES.DLL", "CAPPLRES.DLL", "CASN4RES.DLL",
       "CITOHRES.DLL", "CN10VRES.DLL", "CN13JRES.DLL", "CN330RES.DLL", "CN33JRES.DLL",
       "CNBJCRES.DLL", "CNEPKRES.DLL", "CNL2JRES.DLL", "CNL4JRES.DLL", "CNLBPRES.DLL",
       "CNRSTRES.DLL", "CSN46RES.DLL", "CSWINRES.DLL", "CT24RES.DLL",  "CT9RES.DLL",
       "DC24RES.DLL",  "DC9RES.DLL",   "DCLSRES.DLL",  "DICONRES.DLL", "DPCRES.DLL",
       "ECP2ERES.DLL", "EP24RES.DLL",  "EP24TRES.DLL", "EP9RES.DLL",   "EPAGCRES.DLL",
       "EPAGERES.DLL", "EPCL5RES.DLL", "EPE2JRES.DLL", "EPEPCRES.DLL", "EPLQTRES.DLL",
       "EPRSTRES.DLL", "EPSNJRES.DLL", "ESCP2RES.DLL", "EXP24RES.DLL", "FMLBPRES.DLL",
       "FU24RES.DLL",  "FU9RES.DLL",   "FUDPCRES.DLL", "FUEPJRES.DLL", "FUPRJRES.DLL",
       "FUXLRES.DLL",  "FX5ERES.DLL",  "FXARTRES.DLL", "FXMEJRES.DLL", "GW63CRES.DLL",
       "HPDJJRES.DLL", "HPDJRES.DLL",  "HPPJRES.DLL",  "HPQJRES.DLL",  "HPTJRES.DLL",
       "IB238RES.DLL", "IB239RES.DLL", "IB52RES.DLL",  "IB557RES.DLL", "IB87WRES.DLL",
       "IBMPTRES.DLL", "IBP24RES.DLL", "IBPPDRES.DLL", "IBPRORES.DLL", "IBPS1RES.DLL",
       "IBQWRES.DLL",  "JP350RES.DLL", "KPDLRES.DLL",  "KSKRES.DLL",   "KSSMKRES.DLL",
       "KYORES.DLL",   "KYP5JRES.DLL", "KYRARES.DLL",  "LGGDIRES.DLL", "LGGS6RES.DLL",
       "LMIKJRES.DLL", "LMPCLRES.DLL", "LX238RES.DLL", "LXFMPRES.DLL", "LXINKRES.DLL",
       "MINOLRES.DLL", "MT735RES.DLL", "MT90RES.DLL",  "MTBJRES.DLL",  "MTLTRES.DLL",
       "MTPCLRES.DLL", "MTY24RES.DLL", "MTY9RES.DLL",  "NC11JRES.DLL", "NC21JRES.DLL",
       "NC24RES.DLL",  "NC24TRES.DLL", "NC62JRES.DLL", "NC70JRES.DLL", "NC75JRES.DLL",
       "NC82JRES.DLL", "NCB2JRES.DLL", "NCDLJRES.DLL", "NCMWJRES.DLL", "NCNMJRES.DLL",
       "NCPCLRES.DLL", "NCT3JRES.DLL", "NCT4JRES.DLL", "OD9IBRES.DLL", "OK21JRES.DLL",
       "OK9IBRES.DLL", "OKD24RES.DLL", "OKEPJRES.DLL", "OKI24RES.DLL", "OKI9RES.DLL",
       "OKIPAGE.DLL",  "OKISCRES.DLL", "OKM24RES.DLL", "OKM4JRES.DLL", "OKML9RES.DLL",
       "OL24RES.DLL",  "OL9RES.DLL",   "OLD24RES.DLL", "OLD9RES.DLL",  "OPTEURES.DLL",
       "OPTRARES.DLL", "PA24RES.DLL",  "PA24TRES.DLL", "PA24W9X.DLL",  "PA9RES.DLL",
       "PA9W9X.DLL",   "PAGESRES.DLL", "PCL3KRES.DLL", "PCL4JRES.DLL", "PCL4KRES.DLL",
       "PCL4RES.DLL",  "PCL5ERES.DLL", "PCL5JRE2.DLL", "PCL5JRES.DLL", "PCL5KRES.DLL",
       "PCL5TRES.DLL", "PCL5URES.DLL", "PCL5ZRES.DLL", "PCLEURES.DLL", "PCLXL.DLL",
       "PLOTTER.DLL",  "PLOTUI.DLL",   "PLOTUI.HLP",   "CNBJMON.DLL",  "ALPSRES.DLL",
       "PPDSKRES.DLL", "QBJRES.DLL",   "QPLKRES.DLL",  "RIAFRES.DLL",  "RIAFUI1.DLL",
       "RIAFUI2.DLL",  "RICOHRES.DLL", "RPDLCFG.DLL",  "RPDLRES.DLL",  "SEK24RES.DLL",
       "SEK9RES.DLL",  "SKCOLRES.DLL", "SML8XRES.DLL", "ST21JRES.DLL", "ST24CRES.DLL",
       "ST24ERES.DLL", "ST24TRES.DLL", "STAR9RES.DLL", "STEPJRES.DLL", "STJTRES.DLL",
       "STNMJRES.DLL", "STR24RES.DLL", "STR9ERES.DLL", "TI850RES.DLL", "TS21JRES.DLL",
       "TSE3JRES.DLL", "TSEPJRES.DLL", "TY2X3RES.DLL", "TY2X4RES.DLL", "WP24RES.DLL",
       "WP9RES.DLL",   "XRPR6RES.DLL", "YJLBPRES.DLL",
    ); 


#---------------------------------------------------#
# Routine for checking device class specific
# directives that occur in common sections verified
# by ChkINF.  Return 0 if directive is unknown or
# 1 if the directive is known (you should also verify
# it and write any appropriate warnings/errors.
#---------------------------------------------------#
sub DCDirective
{
    # first parameter is module name - discard it
    shift;

    # Print specific directives
    my %ValidPrintDirectives = ("CONFIGFILE"        => 1,
                                "DATAFILE"          => 1,
                                "DRIVERFILE"        => 1,
                                "DEFAULTDATATYPE"   => 1,
                                "HELPFILE"          => 1,
                                "LANGUAGEMONITOR"   => 1,
                              # "EXCLUDEFROMSELECT" => 1, put in [ControlFlags] section, not here
                                "PRINTPROCESSOR"    => 1,
                                "DATASECTION"       => 1,
                                "VENDORSETUP"       => 1
                               );

    # directive in question
    my $query = shift;

    # if defined, return true, else false
    return (defined $ValidPrintDirectives{uc $query});
}

#---------------------------------------------------#
# The module's main checking routine.
# This is your entry point for verifying an INF
#---------------------------------------------------#
sub InfCheck
{
    my $Item;
    my $Model;
    my $aref_IDs;

    my $StartTime = time() if ($DEBUG);

    print(STDERR "\tInvoking Printer::InfCheck...\n") if ($DEBUG);
    
    $NTPrintINF_Included = IsNTPrintINF_Included();
    print (STDERR "\tINF ", $NTPrintINF_Included ? "includes" : "does not include", " NTPrint.inf\n") if ($DEBUG);

    $LayoutINF_Included = IsLayoutINF_Included();
    print (STDERR "\tINF ", $LayoutINF_Included ? "includes" : "does not include", " Layout.inf\n") if ($DEBUG);

    # Get list of values in [ControlFlags] InteractiveInstall.  Duplicate Pnp HWid's 
    #    error messages are suppressed if duplicate HWid is in this list, since the user
    #    is prompted to choose the right model.
    BuildInteractiveInstallList();

    # A hash of all models (sections) found in the INF.  Values are a ref to array of ID's
    %AllModels = LearnModels();

    # Process each model.
    # (use 'each' instead of 'keys' because it is less memory intensive and we need
    #    the value for the debug statement anyway.)
    while (($Model, $aref_IDs) = each(%AllModels))
    {
        # $Model    is Model section
        # $aref_IDs is ref to array of HW PNP-ID and Compatible IDs
        print(STDERR "===================================================================\n") if ($DEBUG);
        print(STDERR "Model Section: $Model  [", join (", ", @{$aref_IDs}), "]\n") if ($DEBUG);

        # Verify the Orig section (decorated, else not decorated)
        my $Decor;
        my $OrigSectionFound = 0;

        foreach $Decor (@SectionDecorations)
        {
            # Verify sections strictly for NT
            if ( $Current_INF_File->OrigSectionDefined( $Model . $Decor ) )
            {
                CheckInstallSection( $Model . $Decor );
                $OrigSectionFound = 1;
            }
        }
        
        if (   (!$OrigSectionFound)
            && ($Current_INF_File->OrigSectionDefined( $Model ) ) )
        {
            # Only verify un-decorated if no NT/2000 specific sections are defined
            if (CheckInstallSection( $Model ))
            {
                $OrigSectionFound = 1;
            }
        }

        if ($OrigSectionFound)
        {
            #   This is the section name from Model=[install section],...., but at the section definition
            
            # Set error if also defined in NTPRINT.INF
            # 8532 => ["Printer install section [%s] conflicts with NTPrint.inf section of the same name."],
            AddPrinterError(8532, $Current_INF_File->getIndex($Model)-1, $Model)
                if (# $NTPrintINF_Included && 
                    !ChkInf::GetGlobalValue("MS_INTERNAL") &&
                    NTPrintDecoratedSectionDefined($Model));

            # Mark special section as referenced, if it exists.  These sections are
            #   directly read by the class installer.
            if ($Current_INF_File->OrigSectionDefined($Model . ".ProcessPerPnpInstance"))
            {
                ChkInf::Mark($Model . ".ProcessPerPnpInstance");
            }
        }
        else
        {
            # Set error if not defined in Original INF.
            #   Can't log here since there is no section to log it against.
            #   Logged as 8524 error in ExpandModelSection
        }
    }

    print(STDERR "===================================================================\n") if ($DEBUG);

    if ($Current_INF_File->OrigSectionDefined("Previous Names"))
    {
        CheckPreviousNames("Previous Names");
    }

    if ($Current_INF_File->OrigSectionDefined("OEM URLs"))
    {
        CheckOEM_URLs("OEM URLs");
    }

    if ($Current_INF_File->OrigSectionDefined("OEMFiles"))
    {
        CheckOEMFiles("OEMFiles");
    }

    if ($Current_INF_File->OrigSectionDefined("DestinationDirs"))
    {
        CheckDestinationDirs("DestinationDirs");
    }
    
    if ($Current_INF_File->OrigSectionDefined("ControlFlags"))
    {
        CheckControlFlags("ControlFlags");
    }

    # If processing an internal INF, then look at all the [xxxx.OEM] sections that
    #   may be present.  These are may not be otherwise referenced internally.
    #   Vendor INFs can reference these using INCLUDES=/NEEDS= directives.
    if (ChkInf::GetGlobalValue("MS_INTERNAL"))
    {
        my @NEEDS_OEM_Sections = grep (/\.OEM$/, $Current_INF_File->getSections());
        my $Section_OEM;
        foreach $Section_OEM (@NEEDS_OEM_Sections)
        {
            ChkInf::Mark($Section_OEM);
        }
        foreach $Section_OEM (("UNIDRV_BIDI_DATA", "UNIDRV_DATA", "PSCRIPT_DATA"))
        {
            ChkInf::Mark($Section_OEM) if $Current_INF_File->sectionDefined($Section_OEM);
        }
    }

    # Calculate and print runtime
    my($Elapsed) = time() - $StartTime if ($DEBUG);

    printf(STDERR "\tFinished Printer::InfCheck (%dh %02dm %02ds)\n",
           int($Elapsed / 3600), int($Elapsed / 60) % 60, $Elapsed % 60) if ($DEBUG);

    return(1); # return 1 on success
}

#---------------------------------------------------#
# Verifies that the module loaded correctly.  Be sure
# to change the string to the name of your module.
#---------------------------------------------------#
sub Exists
{
    print(STDERR "Module \"PRINTER\" Loaded\n");
    return(1); # return 1 on success
}

#---------------------------------------------------#
# Is called when module is first included.
# Use it to set your per-inf variables.
#---------------------------------------------------#
sub InitGlobals
{
    my $StartTime = time() if ($DEBUG);

    print(STDERR "\tInvoking Printer::InitGlobals...\n") if ($DEBUG);
    # First parameter is the INF object
    $Current_INF_File=$_[1];

    # Second parameter is a list of Device Specific options.
    my($DC_Specific_Options) = $_[2];

    # $DC_Specific_Options is a string that contains all device
    #   class options delimited by &'s
    if (defined($DC_Specific_Options)) {
        @Device_Class_Options = split(/&/,$DC_Specific_Options);
    } else {
        @Device_Class_Options = ("NULL", "NULL");
    }

    # Array must be shifted since first element also had a & prepended
    shift(@Device_Class_Options);

    %AllModels = ();
    %CurrentINFModelHWids = ();
    %CurrentINFModelNames = (); 
    %CurrentINFManufacturerNames = ();
    @InteractiveInstallPnpIDs = ();

    # Need direct access to NTPrint.inf to verify some data.  Let ChkINF
    #   infrastructure handle most of the messy work.
    # Preserve data when invoked more than once since NTPrint.INF is static.
    # Same for Layout.inf.
    if (!defined(%NTPrintINFModelNames))
    {
        $NTPrintINF_File      = InfFile->new($NTPrintINF_path);
        print(STDERR "\tDone reading NTPrint.inf. Handle is $NTPrintINF_File\n") if ($DEBUG);
        print(STDERR "\tBuilding NTPrint.inf model list ...\n") if ($DEBUG);
#        LearnStrings("Strings"); # adds strings to global list - undesireable
        %NTPrintINFModelNames = LearnNTPrintModels();
        print(STDERR "\tBuilding NTPrint.inf model list ... Done, (", scalar(keys %NTPrintINFModelNames)," models)\n") if ($DEBUG);
        print(STDERR "\tBuilding NTPrint.inf SourceDisksFiles list ...\n") if ($DEBUG);
        @NTPrintSDFiles       = LearnSDFilesInFile($NTPrintINF_File);
        print(STDERR "\tBuilding NTPrint.inf SourceDisksFiles list ... Done, (", scalar(@NTPrintSDFiles), " files)\n") if ($DEBUG);

        $LayoutINF_File       = InfFile->new($LayoutINF_path);
        print(STDERR "\tDone reading Layout.inf. Handle is $LayoutINF_File\n") if ($DEBUG);
        print(STDERR "\tBuilding Layout.inf SourceDisksFiles list ...\n") if ($DEBUG);
        @LayoutSDFiles        = LearnSDFilesInFile($LayoutINF_File);
        print(STDERR "\tBuilding Layout.inf SourceDisksFiles list ... Done, (", scalar(@LayoutSDFiles), " files)\n") if ($DEBUG);
    }

    $NTPrintINF_Included = 0;   # set if INF explicitly includes NTPrint.INF
    $LayoutINF_Included  = 0;   # set if INF explicitly includes Layout.INF
    
    @SDFiles = (); # rebuild this list (on first use) for each INF

    print(STDERR "\t         Printer::InitGlobals...Done (", time() - $StartTime, " secs)\n") if ($DEBUG);

    return(1); # return 1 on success
}


#---------------------------------------------------#
# Allows to add Device specific information to the
# top of the INF summary page.  The information here
# should be brief and link to detailed summaries
# below. (Which will be written using PrintDetials).
#---------------------------------------------------#
sub PrintHeader
{
    return(1); # return 1 on success
}


#---------------------------------------------------#
# Allows addition of device specific results to the
# detail section on the INF summary.
#---------------------------------------------------#
sub PrintDetails
{
    return(1); # return 1 on success
}

#-------------------------------------------------------------------------------------------------#
#-- AddPrinterError($ErrorNum, $LineNum, @ErrorArgs) ---------------------------------------------#
sub AddPrinterError
{
    my($ErrorNum) = shift;
    my($LineNum)  = shift;
    my(@ErrorArgs)= @_;

    my %ErrorTable = (  
        8521 => ["No CONFIGFILE directive was found in DDInstall section or referenced DATASECTION."],
        8522 => ["No DRIVERFILE directive was found in DDInstall section or referenced DATASECTION."],
        8523 => ["No DATAFILE directive was found in DDInstall section, referenced DATASECTION, or via implied section name."],
        8524 => ["Printer Model \"%s\" install section [%s] not defined in this INF."],
        8525 => ["Printer Model \"%s\" install section [%s] conflicts with NTPrint.inf defined section of the same name."],
        8526 => ["DataSection directive %s may only occur once."],
        8527 => ["DDInstall section directive %s may only occur once."],
        8528 => ["DDInstall section and referenced DATASECTION both define a %s directive."],
        8529 => ["Invalid format.  Should be of the form: %s"],
        8530 => ["%s \"%s\"  is not found in any SourceDisksFiles section."],
        8531 => ["%s \"%s\" is not found in a SourceDisksFiles section in this INF."],
        8532 => ["Printer install section [%s] conflicts with NTPrint.inf section of the same name."],
        8533 => ["Printer Model \"%s\" install section [%s] occurs more than once."],
        8534 => ["Install section [%s] occurs more than once."],
        8535 => ["Printer Model \"%s\" install section [%s] exists with some but not all valid CPU decorated forms (".join (", ", @NTCpuDecorations).")"],
        8536 => ["\"%s\" is not a model name listed in this INF."],
        8537 => ["Invalid format.  No previous names listed."],
        8538 => ["Invalid format.  Missing previous name entry (extra commas?)."],
        8539 => ["Previous name \"%s\" is a currently active model name (referenced in NTPrint.inf)."],
        8540 => ["Do not copy file %s directly, use INCLUDE=NTPrint.inf & appropriate NEEDS=."],
        8541 => ["File %s is not listed in the [SourceDisksFiles] section(s) of this INF."],
        8542 => ["File %s conflicts with file shipped in the OS (referenced in %s)."],
        8543 => ["Install section [%s] not defined in this INF."],
        8544 => ["Install section [%s] conflicts with NTPrint.inf section of the same name (INCLUDE=NTPrint.inf exists)."],
        8545 => ["File %s ships with the OS.  Do not list it in the [SourceDisksFiles] section(s) of this INF.  Use %s instead."],
        8546 => ["File %s ships with the OS.  Use %s to reference it."],
        8547 => ["\"%s\" is not an OEM (Manufacturer) name listed in this INF."],
        8548 => ["\"%s\" is not recognized (not OEMDriverFile[1-9] or OEMConfigFile[1-9])."],
        8549 => ["%s is improperly numbered, expected %s%d."],
        8550 => ["\"%s\" is not a DLL name."],
        8551 => ["\"%s\" is not an ID listed in this INF."],
        8552 => ["Missing '\\' after Pnp Bus enumerator '%s'."],
        8553 => ["Apparent Pnp Bus enumerator '%s' is not recognized as a valid enumerator (".join (", ", @PnpBusPrefixes).")."],
        8554 => ["Install section [%s] conflicts with section [%s] given earlier for model '%s'."],
        8555 => ["Previous name \"%s\" is an active model name listed in this INF."],
        8556 => ["PnP Hardware ID [%s] conflicts with HWid for model '%s'."],
    );

    $ErrorArgs[0] = " " if (! defined($ErrorArgs[0]));
    my($str, $this_str, $info_err, $format_err);

    $info_err = $ErrorTable{"$ErrorNum"};

    if ( !defined($info_err) )
    {
        $this_str = "Unknown error $ErrorNum.";
    }
    else
    {
        $format_err = $$info_err[0];
        $this_str = sprintf($format_err, @ErrorArgs);
    }

    ChkInf::AddDeviceSpecificError($LineNum, $ErrorNum, $this_str);
    print (STDERR "E $ErrorNum : $this_str\n") if $DEBUG > 1;
}

#-------------------------------------------------------------------------------------------------#
#-- AddPrinterWarning($ErrorNum, $LineNum, @ErrorArgs) -------------------------------------------#
sub AddPrinterWarning
{
    my($ErrorNum) = shift;
    my($LineNum)  = shift;
    my(@ErrorArgs)= @_;

    my %ErrorTable = (
        8501 => ["Manufacturer \"%s\" has no printer models defined."],
        8502 => ["Manufacturer \"%s\" has no printer models defined in this INF."],
        8503 => ["Use of DirID %s prevents full remote/point-and-print installation."],
        8504 => ["Printer Model \"%s\" also exists in NTPrint.inf."],
        8505 => ["DirID %s=%s is discouraged.  Should be in 66000-66004 range in order to support remote install."],
        8506 => ["Use of Bus enumerator '%s' on PnpID is discouraged, just '%s' preferred."],
        8507 => ["Install section [%s] conflicts with NTPrint.inf section of the same name."],
        8508 => ["\"%s\" listed more than once."],
        8509 => ["Do not reference section '%s' directly, use INCLUDE=NTPrint.inf & appropriate NEEDS=."],
        8510 => ["Duplicate section reference '%s' in NEEDS="],
        # 8511 => ["DATASECTION reference '%s' is also included via NEEDS=.  Only one is necessary (NEEDS preferred)."],
        # 8512 => ["DATASECTION reference '%s' can be replaced with a corresponding INCLUDE=NTPrint.inf & NEEDS=%s entries."],
        8512 => ["DATASECTION reference '%s' should have a corresponding INCLUDE=NTPrint.inf & NEEDS=%s entry."],
        8513 => ["Model name \"%s\" and Previous name \"%s\" are the same%s."],
    );

    $ErrorArgs[0] = " " if (! defined($ErrorArgs[0]));
    my($str, $this_str, $info_err, $format_err);

    $info_err = $ErrorTable{"$ErrorNum"};

    if ( !defined($info_err) )
    {
        $this_str = "Unknown error $ErrorNum.";
    }
    else
    {
        $format_err = $$info_err[0];
        $this_str = sprintf($format_err, @ErrorArgs);
    }

    ChkInf::AddDeviceSpecificWarning($LineNum, $ErrorNum, $this_str);
    print (STDERR "W $ErrorNum : $this_str\n") if $DEBUG > 1;
}

#-------------------------------------------------------------------------------------------------#
#-- PrinterDecoratedSectionDefined($Section) -----------------------------------------------------#
sub PrinterDecoratedSectionDefined
{
    my $Section = shift;
    my $Decor;

    foreach $Decor (@SectionAllDecorations) {
        return 1 if ($Current_INF_File->sectionDefined($Section . $Decor));
    }
    return 0;
}

#-------------------------------------------------------------------------------------------------#
#-- PrinterDecoratedOrigSectionDefined($Section) -------------------------------------------------#
sub PrinterDecoratedOrigSectionDefined
{
    my $Section = shift;
    my $Decor;
    
    foreach $Decor (@SectionAllDecorations) {
        print (STDERR "\t\t   OrigSection '$Section$Decor' is ") if ($DEBUG > 1);
        if ($Current_INF_File->OrigSectionDefined($Section . $Decor))
        {
            print (STDERR "defined\n") if ($DEBUG > 1);
            # Now make sure it is a real section (physically exists), not just 
            # a 'single file' section.  A 'real' section has lines associated with it.
            my @xlines = $Current_INF_File->getOrigSection($Section . $Decor);
            return (scalar(@xlines) > 0);
        }
        else
        {
            print (STDERR "NOT defined\n") if ($DEBUG > 1);
        }
    }
    return 0;
}

#-------------------------------------------------------------------------------------------------#
# PrinterOrigSectionDefined - Returns true if the section is truly defined in the current INF and #
#   not included via a NEEDS reference.                                                           #
#-- PrinterOrigSectionDefined($Section) ----------------------------------------------------------#
sub PrinterOrigSectionDefined
{
    my $Section     = shift;
    my $NumSections;
    my $idx;
    my $InfFile;

    if ($Current_INF_File->OrigSectionDefined($Section))
    {
        return (uc($Current_INF_File->{"inffile"}) eq uc($Current_INF_File->instanceData($Section, 0, "inffile")));
    }

    return 0;
}

#-------------------------------------------------------------------------------------------------#
# IsLayoutINF_Included - Returns true if the INF includes the system Layout.inf                   #
#-- IsLayoutINF_Included() ----------------------------------------------------------------------#
sub IsLayoutINF_Included
{
    return (ChkInf::GetGlobalValue("LAYOUTFILE_USED") == ChkInf::TRUE());
}

#-------------------------------------------------------------------------------------------------#
# IsNTPrintINF_Included - Returns true if the INF includes the system NTPrint.inf                 #
#-- IsNTPrintINF_Included() ----------------------------------------------------------------------#
sub IsNTPrintINF_Included
{
    print (STDERR "INCLUDED_FILES=@{$Current_INF_File->{_INCLUDED_FILES_}}\n") if ($DEBUG > 1);
    return (0 < grep( /^\Q$NTPrintINF_path\E$/i , @{$Current_INF_File->{_INCLUDED_FILES_}} ));
}

#-------------------------------------------------------------------------------------------------#
# NTPrintDecoratedSectionDefined - Returns true if the INF included NTPrint.inf and if the (maybe #
#   decorated) section exists in NTPrint.inf.                                                     #
#   Section name passed in is assumed to be undecorated.  Possible decorations are added.         #
#
#   This check is provided because installed OEM INFs are combined with NTPrint.INF when the      #
#   AddPrinter Wizard brings up the list of available printer models - section conflicts could    #
#   cause improper behaviour (OEM files being installed when MS model selected, or vice-versa.    #
#-- NTPrintDecoratedSectionDefined($Section) -----------------------------------------------------#
sub NTPrintDecoratedSectionDefined
{
   my $Section = shift;
   my $Decor;

   print (STDERR "\tInvoking NTPrintDecoratedSectionDefined ($Section) ....\n") if ($DEBUG > 1);

   foreach $Decor (@SectionAllDecorations)
   {
       return 1 if (NTPrintSectionDefined($Section . $Decor));
   }

   return (0);
}

#-------------------------------------------------------------------------------------------------#
# NTPrintSectionDefined - Returns true if the section is defined in NTPrint.inf.  The section     #
#   name is checked exactly as passed in.  Call IsNTPrintINF_Included() first if it is necessary  #
#   to know if NTPrint.INF is explicitly included by the Current INF.                             #
#
#   This check is provided because installed OEM INFs are combined with NTPrint.INF when the      #
#   AddPrinter Wizard brings up the list of available printer models - section conflicts could    #
#   cause improper behaviour (OEM files being installed when MS model selected, or vice-versa.    #
#-- NTPrintSectionDefined($Section) --------------------------------------------------------------#
sub NTPrintSectionDefined
{
    # This uses our private version of NTPrint.INF data and does not rely on the current
    #   INF having INCLUDEd NTPrint.inf.

    return ($NTPrintINF_File->sectionDefined(shift));
}

#-------------------------------------------------------------------------------------------------#
# NTPrintSectionDefined - Returns true if the INF included NTPrint.inf and if the section is      #
#   defined in NTPrint.inf.  The section name is checked exactly as passed in.                    #
#-- NTPrintSectionDefined($Section) --------------------------------------------------------------#
#sub NTPrintSectionDefined
#{
#    my $Section     = shift;
#    my $NumSections;
#    my $idx;
#    my $InfFile;
#
#    if ($Current_INF_File->sectionDefined($Section))
#    {
#        $NumSections = $Current_INF_File->count($Section);  # zero-based value
#
#        for ($idx=0; $idx <= $NumSections; $idx++ )
#        {
#            $InfFile = $Current_INF_File->instanceData($Section, $idx, "inffile");
#            return 1 if ($InfFile =~ /NTPRINT.INF/i);
#        }
#    }
#
#    return 0;
#}

#-------------------------------------------------------------------------------------------------#
# NTPrintModelNameExists - Returns true if the Model name exists in NTPrint.inf                   #
#-- NTPrintModelNameExists($ModelName) -----------------------------------------------------------#
sub NTPrintModelNameExists
{
    my $ModelName = shift;
    return (exists($NTPrintINFModelNames{uc $ModelName}));
}

#-------------------------------------------------------------------------------------------------#
# DuplicatedInstallSection - Returns true if the section exists more than once in the current INF #
#-- DuplicatedInstallSection($Section) -----------------------------------------------------------#
sub DuplicatedInstallSection
{
    return CheckForDuplicateInstallSection (shift, 0);
}

#-------------------------------------------------------------------------------------------------#
# CheckForDuplicateInstallSection - Returns true if the section exists more than once in the      #
#   current INF, and optionally logs an error against each instance (if $LogError is true).       #
#-- CheckForDuplicateInstallSection($Section, $LogError) -----------------------------------------#
sub CheckForDuplicateInstallSection
{
    my $Section     = shift;
    my $LogError    = shift;
    my $NumSections;
    my $idx;
    my $InfFile;
    my $OrigInfFile;
    my $DupFound    = 0;

    # assumes Original INF is first INF listed
    # ChkINF master processing handles dup's where decorations conflict.

    if ($Current_INF_File->OrigSectionDefined($Section))
    {
        $NumSections = $Current_INF_File->count($Section);  # zero-based value

        $OrigInfFile = $Current_INF_File->instanceData($Section, 0, "inffile");

        for ($idx=1; $idx <= $NumSections; $idx++ )
        {
            $InfFile = $Current_INF_File->instanceData($Section, $idx, "inffile");
            if ($InfFile eq $OrigInfFile)
            {
                $DupFound = 1;
                last unless ($LogError);

                # 8534 => ["Install section [%s] occurs more than once."],
                AddPrinterError(8534, $Current_INF_File->instanceData($Section, $idx, "fileoffset")-1, $Section);
            }
        }
    }

    if ($DupFound && $LogError)
    {
        # 8534 => ["Install section [%s] occurs more than once."],
        AddPrinterError(8534, $Current_INF_File->getIndex($Section)-1, $Section);
    }

    return $DupFound;
}

#-------------------------------------------------------------------------------------------------#
# AllOrNoCpuDecorations - Returns true if the section exists with all valid CPU decorations or    #
#   with none of them (must have all or none).                                                    #
#-- AllOrNoCpuDecorations($Section) --------------------------------------------------------------#
sub AllOrNoCpuDecorations
{
    my $Section  = shift;
    my @CpuDecor = @NTCpuDecorations;
    my $Decor;
    my $result   = $Current_INF_File->OrigSectionDefined($Section . shift(@CpuDecor));

    foreach $Decor (@CpuDecor)
    {
        return 0 if ($result != $Current_INF_File->OrigSectionDefined($Section . $Decor));
    }

    return (1);
}


#-------------------------------------------------------------------------------------------------#
# LearnNTPrintModels - Returns an array of all models listed in NTPrint.INF                       #
#-- LearnNTPrintModels() -------------------------------------------------------------------------#
sub LearnNTPrintModels
{
    my $Section = "Manufacturer";
    my $line;
    my @lines   = $NTPrintINF_File->getSection($Section);
    my $count   = 0;

    my($Directive, $Value);
    my(%ModelNames);

    print(STDERR "\tInvoking Printer::LearnNTPrintModels...\n") if ($DEBUG);

    foreach $line (@lines)
    {
        $count++, next if ($line =~ /^\s*$/); # Skip if it's a blank line

##### Don't use SplitLine - it wants to resolve string tokens (which issues errors here)

        # ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);
        ($Directive,$Value) = split(/=/, $line, 2);

        # [Manufacturers] may have both a directive & a value if the Directive is
        #   really a replaceable string value and the Value is the real section name.
        if ($Value =~ /^\s*$/)
        {
            $Value     = $Directive;
            $Directive = "";
        }
        else
        {
            $Value     =~  s/^\"(.*)\"/$1/;   # And Strip the Quotes!
        }
        print(STDERR "Manufacturer: $Value") if ($DEBUG > 1);  # NO \n character!

        # Add the model names from $INFSections{$Value} to our running list
        %ModelNames = (%ModelNames, ExpandNTPrintModelSection($Value));
        $count++;
    }
    return(%ModelNames);
}

#-------------------------------------------------------------------------------------------------#
#-- ExpandNTPrintModelSection($Section) ----------------------------------------------------------#
sub ExpandNTPrintModelSection
{
    my $Section  = shift;
    my $line;
    my @lines    = $NTPrintINF_File->getSection($Section);
    my $count    = 0;

    my %ModelNames = ();

    my($Directive, @Values, $Value);

    foreach $line (@lines)
    {
       $count++, next if ($line =~ /^\s*$/);# Skip if it's a blank line

##### Don't use SplitLine - it wants to resolve string tokens (which issues errors here)
##### Don't use GetList - it wants to resolve string tokens (which issues errors here)
        # ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);
        ($Directive,$Value) = split(/=/, $line, 2);
        # @Values             = ChkInf::GetList($Value,$NTPrintINF_File->getIndex($Section,$count));
        @Values             = split(/,/, $Value, 2);

        $Directive =~ s/^\s*\"*//; # Kill leading spaces and quotes
        $Directive =~ s/\"*\s*$//; # Kill trailing spaces and quotes
        $Values[0] =~ s/^\s*//; # Kill leading spaces
        $Values[0] =~ s/\s*$//; # Kill trailing spaces

        $count++, next if ($Values[0] eq ""); # Skip if no value

        if (defined($Values[0]))
        {
            $ModelNames{uc $Directive} = $Values[0];
            print(STDERR ".") if ($DEBUG > 2);
        }
        $count++;
    }
    print(STDERR " (", scalar(keys(%ModelNames)), " Models)\n") if ($DEBUG > 1);

    return(%ModelNames);
}

#-------------------------------------------------------------------------------------------------#
# LearnSDFilesInFile - Returns an array of lines in the SourceDisksFiles section(s) of file specified #
#-- LearnSDFilesInFile($filehandle) --------------------------------------------------------------#
sub LearnSDFilesInFile
{
    my $File = shift;
    my @Decorations = (".x86", ".IA64", ".AMD64", "");  # listed in priority order
    my $Decor;
    my @SDF;
    
    # Build list of SourceDisksFiles, undecorated and decorated.  I think SetupAPI's
    # actually look for decorated sections first (regardless of which INF they are in)
    # so build list in that order.
    foreach $Decor (@Decorations) {
        if ( $File->sectionDefined("SOURCEDISKSFILES" . $Decor) ) {
            @SDF = (@SDF, $File->getSection("SourceDisksFiles" . $Decor));
        }
    }

    # print (STDERR "File SDFiles: \n"); }
    # foreach (@SDF) { print (STDERR "\t$_\n"); }

    # Don't (currently) have the string tokens available, so can't resolve them.
    #  Besides, NTPrint/Layout.inf don't use tokens in the SDFiles sections.
    # process entire set for string tokens (if there is anything to process)
    # if ($#SDF > 0)
    # {
    #     map( {$_=RemoveStringTokens(-1,$_);} @SDF );
    # }

    return @SDF;
}

#-------------------------------------------------------------------------------------------------#
# LearnModels - Returns an array of all models listed in the INF                                  #
#-- LearnModels() --------------------------------------------------------------------------------#
sub LearnModels
{
    my $Section = "Manufacturer";
    # If the Original INF has include= lines, any like named sections are merged.
    # We are only interested in the original INF's list.
    return(0) if (! $Current_INF_File->OrigSectionDefined($Section));
    my $line;
    my @lines   = $Current_INF_File->getOrigSection($Section);
    my $count   = 0;

    my($Directive, $Value);
    my %Models;

    print(STDERR "\tInvoking Printer::LearnModels...\n") if ($DEBUG);

    foreach $line (@lines)
    {
        $count++, next if ($line =~ /^\s*$/); # Skip if it's a blank line

        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);

        # [Manufacturers] may have both a directive & a value if the Directive is
        #   really a replaceable string value and the Value is the real section name.
        if ($Value =~ /^\s*$/)
        {
            $Value     = $Directive;
            $Directive = "";
        }
        else
        {
            $Value     =~  s/^\"(.*)\"/$1/;   # And Strip the Quotes!
        }
        print(STDERR "==================================================================\n") if ($DEBUG);
        print(STDERR "Manufacturer: $Value\n") if ($DEBUG);

        $CurrentINFManufacturerNames{uc $Directive} = $Value;

        if ( $Current_INF_File->OrigSectionDefined($Value) )
        {
            if ($Current_INF_File->getOrigSection($Value) != ())
            {
                # Add the models from $INFSections{$Value} to our running list
                %Models = (%Models, ExpandModelSection($Value));
            }
            else
            {
                # if no manufacturer specific section in this INF, show warning.
                # 8502 => ["Manufacturer \"%s\" has no printer models defined in this INF."],
                AddPrinterWarning(8502, $Current_INF_File->getIndex($Section,$count), $Value);
            }
        }
        else
        {
            # if no manufacturer specific section found anywhere, show warning.
            # 8501 => ["Manufacturer \"%s\" has no printer models defined."],
            AddPrinterWarning(8501, $Current_INF_File->getIndex($Section,$count), $Value);
        }
        $count++;
    }

    return(%Models);
}

#-------------------------------------------------------------------------------------------------#
#-- ExpandModelSection($Section) -----------------------------------------------------------------#
sub ExpandModelSection
{
    my $Section  = shift;
    my $line;
    my @lines    = $Current_INF_File->getOrigSection($Section);
    my $count    = 0;

    my %Models   = ();

    my $Directive;
    my $Value;
    my $SectionName;

    print(STDERR "\tInvoking Printer::ExpandModelSection...\n") if ($DEBUG);

    foreach $line (@lines)
    {
        $count++, next if ($line =~ /^\s*$/);# Skip if it's a blank line

        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);

        # MUST use 'my' here so we get a fresh instance of the array - needed since we keep a 
        #  reference to it as a %Models/%AllModels value
        my @Values          = ChkInf::GetList($Value,$Current_INF_File->getIndex($Section,$count));

        $SectionName = shift(@Values);

        $SectionName =~ s/^\s*//i; # Kill leading spaces
        $SectionName =~ s/\s*$//i; # Kill trailing spaces

        $count++, next if ($SectionName eq ""); # Skip if no value

        print(STDERR "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n") if ($DEBUG);

        if (exists($Models{uc $SectionName}))
        {
            $Models{uc $SectionName} = [@{$Models{uc $SectionName}}, @Values]; # add IDs to IDs already associated with this section - anonymous array *reference*!!!
        }
        else
        {
            $Models{uc $SectionName} = \@Values; # save rest of values (HWid and compatible ID's) - array *reference*!!!
        }
        print(STDERR "Model '$Directive' : '$SectionName' = '", join (", ", @{$Models{uc $SectionName}}), "'\n") if ($DEBUG);

        if (exists($CurrentINFModelNames{uc $Directive}))
        {
            # 8554 => ["Install section [%s] conflicts with section [%s] given earlier for model '%s'."],
            AddPrinterError(8554, $Current_INF_File->getIndex($Section,$count), $SectionName, $CurrentINFModelNames{uc $Directive}, $Directive)
                unless (uc($CurrentINFModelNames{uc $Directive}) eq uc($SectionName));
        }
        else
        {
            $CurrentINFModelNames{uc $Directive} = $SectionName;
        }

        # $Directive (Model names) do not appear to be normally defined!
        #  Check this regardless of NTPrint.INF included or not.
        # 8504 => ["Printer Model \"%s\" also exists in NTPrint.inf."],
        AddPrinterWarning(8504, $Current_INF_File->getIndex($Section,$count), $Directive, $SectionName)
            if (   !ChkInf::GetGlobalValue("MS_INTERNAL")
                && NTPrintModelNameExists($Directive));

        # These sections appear to be defined, even if they don't exist.
        #   This may be a result of the 'is it datafile or install section' logic.
        # Need to check the name against the SDFiles list to determine if true datafile.

        if (PrinterDecoratedSectionDefined($SectionName))
        {
            #   This is the section name from Model=[install section],...., at the model= line
            if (!PrinterDecoratedOrigSectionDefined($SectionName))
            {
                # Flag error if not defined in Original INF
                # 8524 => ["Printer Model \"%s\" install section [%s] not defined in this INF."],
                AddPrinterError(8524, $Current_INF_File->getIndex($Section,$count), $Directive, $SectionName);
            }
            else
            {
                # Flag error if section is defined in NTPrint.INF
                # 8525 => ["Printer Model \"%s\" install section [%s] conflicts with NTPrint defined section of the same name."],
                AddPrinterError(8525, $Current_INF_File->getIndex($Section,$count), $Directive, $SectionName)
                    if (# $NTPrintINF_Included &&
                        !ChkInf::GetGlobalValue("MS_INTERNAL") &&
                        NTPrintDecoratedSectionDefined($SectionName));
            }
        }
        else
        {
            # No install section anywhere
            #  This is already detected by 'master' ChkINF processing
        }

        # Note: This is checking for multiple install sections, not references to the install sections.
        # 8533 => ["Printer Model \"%s\" install section [%s] occurs more than once."],
        AddPrinterError(8533, $Current_INF_File->getIndex($Section,$count), $Directive, $SectionName)
            if (DuplicatedInstallSection($SectionName));

        # 8535 => ["Printer Model \"%s\" install section [%s] exists with some but not all valid CPU decorated forms (".join (", ", @NTCpuDecorations).")"],
        AddPrinterError(8535, $Current_INF_File->getIndex($Section,$count), $Directive, $SectionName)
            unless (AllOrNoCpuDecorations($SectionName));


        # Check for duplicated HWid's
        if (exists($CurrentINFModelHWids{uc $Values[0]}))
        {
            my $q_HWid = quotemeta($Values[0]);
            # 8556 => ["PnP Hardware ID [%s] conflicts with HWid for model '%s'."],
            AddPrinterError(8556, $Current_INF_File->getIndex($Section,$count), $Values[0], $CurrentINFModelHWids{uc $Values[0]})
                unless (   (grep(/^$q_HWid$/i, @InteractiveInstallPnpIDs) > 0) # listed in InteractiveInstall directive
                        || (uc($CurrentINFModelHWids{uc $Values[0]}) eq uc($Directive))  # references same model name
                );
        }
        else
        {
            $CurrentINFModelHWids{uc $Values[0]} = $Directive;
        }

### Will need to revisit this code since PnpBusTypes are required for some instances.
        
        # Flag use of PnpBusPrefixes as depreciated
        my $PnpID;
        my $PnpPrefixes = join("|", map(quotemeta, @PnpBusPrefixes)); # build pattern match string

        foreach $PnpID (@Values)
        {
            if ($PnpID =~ /^($PnpPrefixes)(.)(.*)/i)
            {
                if ($2 ne "\\") # common typo - missing backslash
                {
                    # 8552 => ["Missing '\\' after Pnp Bus enumerator '%s'."],
                    AddPrinterError(8552, $Current_INF_File->getIndex($Section,$count), $1);

                    # 8506 => ["Use of Bus enumerator '%s' on PnpID is discouraged, just '%s' preferred."],
                    AddPrinterWarning(8506, $Current_INF_File->getIndex($Section,$count), $1, $2.$3)
                        unless (ChkInf::GetGlobalValue("MS_INTERNAL"));
                } else
                {
                    # 8506 => ["Use of Bus enumerator '%s' on PnpID is discouraged, just '%s' preferred."],
                    AddPrinterWarning(8506, $Current_INF_File->getIndex($Section,$count), $1, $3)
                        unless (ChkInf::GetGlobalValue("MS_INTERNAL"));
                }
            }
            else  # check for potential typo's
            {
                # 8553 => ["Apparent Pnp Bus enumerator '%s' is not recognized as a valid enumerator (".join (", ", @PnpBusPrefixes).")."],
                AddPrinterError(8553, $Current_INF_File->getIndex($Section,$count), $1)
                    if ($PnpID =~ /^(.*)\\/);
            }
        }

        $count++;
    }

    return(%Models);
}

#-------------------------------------------------------------------------------------------------#
# CheckInstallSection checks the validity of the select section contents.                         #
#    Returns # lines in section (so caller knows the section truely exists.                       #
#-- CheckInstallSection($Section) ----------------------------------------------------------------#
sub CheckInstallSection 
{
    my $Section = shift;
    my @lines   = $Current_INF_File->getOrigSection($Section);
    my $count   = 0;
    my $line;
    my $Directive;
    my $Value;

    my ($DataSectionName, $DataSectionLine);
    my (%NeedsValues);

    my %DataSectionDirectives;

    my %ImportantDirectives = ( "COPYFILES"       => 0,
                                "DATASECTION"     => 0,
                                "DATAFILE"        => 0,
                                "CONFIGFILE"      => 0,
                                "DRIVERFILE"      => 0,
                                "HELPFILE"        => 0,
                                "LANGUAGEMONITOR" => 0,
                                "PRINTPROCESSOR"  => 0,
                                "VENDORSETUP"     => 0,
                                "NEEDS"           => 0,
                              );

    # DCDirective() (called by master ChkINF processing) validates acceptable directives
    #   so no need to do that here.

    print(STDERR "\tInvoking Printer::CheckInstallSection ($Section)...\n") if ($DEBUG);
    ChkInf::Mark($Section); # these appear to be marked already by some earlier processing

    my $DupDetected = CheckForDuplicateInstallSection($Section, 1);

    foreach $line (@lines)
    {
        ($count++, next) if ($line =~ /^\s*$/);# Skip if it's a blank line

        ($Directive, $Value) = ChkInf::SplitLine($Section,$count,$line);

        $Directive = uc $Directive;

        if (defined $ImportantDirectives{$Directive})
        {
            $ImportantDirectives{$Directive}++;
    
            # 8527 => ["DDInstall section directive %s may only occur once."],
            AddPrinterError(8527, $Current_INF_File->getIndex($Section,$count), $Directive)
                if (   ($ImportantDirectives{$Directive} > 1)
                    && ($Directive ne "COPYFILES") # multiple allowed
                    && ($Directive ne "NEEDS") # multiple allowed
                    && (!$DupDetected)); # Duplicates are concatenated so duped directives are likely (but confusing)
    
            if ($Directive eq "COPYFILES")
            {
                CheckCopyFiles($Value, $Current_INF_File->getIndex($Section,$count));
            }
            elsif ($Directive eq "DATASECTION")
            {
                $DataSectionName = $Value;
                $DataSectionLine = $Current_INF_File->getIndex($Section,$count);

                %DataSectionDirectives = CheckDataSection($Value);
            }
            elsif ($Directive eq "NEEDS")
            {
                my $NeedSection;
                my @Values = ChkInf::GetList($Value,$Current_INF_File->getIndex($Section,$count));

                foreach $NeedSection (@Values)
                {
                    if (exists $NeedsValues{uc $NeedSection})
                    {
                        # 8510 => ["Duplicate section reference '%s' in NEEDS="],
                        AddPrinterWarning(8510, $Current_INF_File->getIndex($Section,$count), $NeedSection);
                    }
                    else
                    {
                        $NeedsValues{uc $NeedSection} = $Current_INF_File->getIndex($Section,$count);
                    }
                }
            }
            else # ($Directive eq "DATAFILE" or "CONFIGFILE" or "DRIVERFILE", or ....)
            {
                # Verify the file exists ???? -- not necessary - copyfiles does that.
                # We *should* verify these files are *also* in copyfiles hiearchy.

                VerifyDirectiveFormat($Directive, $Value, $Current_INF_File->getIndex($Section,$count));
            }
        }
        $count++;
    }

    # Check that
    #   If there is a DATASECTION entry
    #   and if this datasection is *really* *not* defined locally
    #   and if NTPrint.inf is included
    #   and if this datasection *is* defined in NTPrint.inf
    # then we prefer a NEEDS reference to it instead.
    if (   (defined($DataSectionName))
        && (!PrinterOrigSectionDefined($DataSectionName))
        && ($NTPrintINF_Included)
        && (NTPrintSectionDefined($DataSectionName)))
    {
        if (exists($NeedsValues{uc $DataSectionName}))
        {
            # Needs only pulls in the files, not the statements, so still need the DATASECTION directive.
            #
            ### # 8511 => ["DATASECTION reference '%s' is also included via NEEDS=.  Only one is necessary (NEEDS preferred)."],
            ### AddPrinterWarning(8511, $DataSectionLine, $DataSectionName);
        }
        else
        {
            # For future compatiblity, recommend having NEEDS line
            #
            ### 8512 => ["DATASECTION reference '%s' can be replaced with a corresponding INCLUDE=NTPrint.inf & NEEDS=%s entries."],
            # 8512 => ["DATASECTION reference '%s' should have a corresponding INCLUDE=NTPrint.inf & NEEDS=%s entry."],
            AddPrinterWarning(8512, $DataSectionLine, $DataSectionName, $DataSectionName);
        }
    }

    # Check for duplicates between DDInstall section and DATASECTION
    foreach $Directive (%ImportantDirectives)
    {
        $Directive = uc $Directive;
        if (   (defined $DataSectionDirectives{$Directive})
            && ($DataSectionDirectives{$Directive} > 0)
            && ($ImportantDirectives{$Directive}   > 0))
        {
            # 8528 => ["DDInstall section and referenced DATASECTION both define a %s directive."],
            AddPrinterError(8528, $Current_INF_File->getIndex($Section)-1, $Directive);
        }
    }
    
    # Ensure DATAFILE has been specified (or implied via section name)
    if (   (   (!defined $DataSectionDirectives{"DATAFILE"})
            || ($DataSectionDirectives{"DATAFILE"} == 0))
        && ($ImportantDirectives{"DATAFILE"}   == 0)
        && (!IsDefinedSourceDisksFile($Section, 0))) # SDF section need not exist
    {
        # If not datafile explicitly listed, then it could be the section name itself.
        #   If the section name is listed in the SourceDiskFile sections, then assume
        #   it is the datafile.  If not, flag error.
        # 8523 => ["No DATAFILE directive was found in DDInstall section, referenced DATASECTION, or via implied section name."],
        AddPrinterError(8523, $Current_INF_File->getIndex($Section)-1);
    }

    # Ensure DRIVERFILE has been explicitly specified
    if (   (   (!defined $DataSectionDirectives{"DRIVERFILE"})
            || ($DataSectionDirectives{"DRIVERFILE"} == 0))
        && ($ImportantDirectives{"DRIVERFILE"}   == 0))
    {
        # 8522 => ["No DRIVERFILE directive was found in DDInstall section or referenced DATASECTION."],
        AddPrinterError(8522, $Current_INF_File->getIndex($Section)-1);
    }

    # Ensure CONFIGFILE has been explicitly specified
    if (   (   (!defined $DataSectionDirectives{"CONFIGFILE"})
            || ($DataSectionDirectives{"CONFIGFILE"} == 0))
        && ($ImportantDirectives{"CONFIGFILE"}   == 0))
    {
        # 8521 => ["No CONFIGFILE directive was found in DDInstall section or referenced DATASECTION."],
        AddPrinterError(8521, $Current_INF_File->getIndex($Section)-1);
    }

    # DATASECTION is not required, just DATAFILE, DRIVERFILE, and CONFIGFILE entries, which
    # are typically defined in a DATASECTION.
    # 85xx => ["Printer DDInstall sections require a DATASECTION entry."],
#    AddPrinterError(85xx, $Current_INF_File->getIndex($Section)-1)
#        unless ($ImportantDirectives{"DATASECTION"} > 0);

    return $count;
}

#-------------------------------------------------------------------------------------------------#
# CheckCopyFiles verifies the referenced files are allowed and not in conflict with files that    #
#   ship with the OS (as listed in NTPrint.inf).  Also verifies the files are listed in the       #
#   SourceDisksFiles section of the original INF (the file *really* exists).                      #
#-- CheckCopyFiles(@CopyFilesValue, $linenum) ----------------------------------------------------#
sub CheckCopyFiles
{
    my @Values  = shift;
    my $linenum = shift;  # for error reporting
    my @CopyFiles;
    my $CopyFile;
    my $idx = 0;

    print(STDERR "\tInvoking Printer::CheckCopyFiles (@Values)...\n") if ($DEBUG);

    for ($idx = 0; $idx < scalar(@Values); $idx++)
    {
        @CopyFiles = split(/,/,$Values[$idx]);
        foreach $CopyFile (@CopyFiles)
        {
            $CopyFile =~ s/^\s*//i; # Kill leading spaces
            $CopyFile =~ s/\s*$//i; # Kill trailing spaces

            next if ($CopyFile =~ /^\s*$/); # Skip if it's a blank name

            # ChkINF master processing verifies that the files/sections referenced
            # by copyfiles exist in SourceDisksFiles (though it verifies it from
            # any SDFiles in any included INF).

            # if @ prefix, then Check file
            # if not @ prefix, then process section lines similarly
            print (STDERR "\t\tCopyfile: $CopyFile\n") if ($DEBUG > 1);

            if ($CopyFile =~ /^@/)
            {
                $CopyFile =~ s/^@//; # strip '@'
                CheckFile($CopyFile, $CopyFile, $linenum);
            }
            else
            {
                my $RefSection = uc $CopyFile;

                if (PrinterDecoratedOrigSectionDefined($RefSection))
                {
                    my @lines = $Current_INF_File->getOrigSection($RefSection);
                    my $count = 0;
                    my $RefCopyFile;

                    #   This is the section name from CopyFiles=...,section,....
                    if (   !ChkInf::GetGlobalValue("MS_INTERNAL")
                        && NTPrintDecoratedSectionDefined($RefSection))
                    {
                        if ($NTPrintINF_Included)
                        {
                            # 8544 => ["Install section [%s] conflicts with NTPrint.inf section of the same name (INCLUDE=NTPrint.inf exists)."],
                            AddPrinterError(8544, $Current_INF_File->getIndex($RefSection)-1, $RefSection);
                        }
                        else
                        {
                            # 8507 => ["Install section [%s] conflicts with NTPrint.inf section of the same name."],
                            AddPrinterWarning(8507, $Current_INF_File->getIndex($RefSection)-1, $RefSection);
                        }
                    }

                    my $MediaFile;
                    foreach $RefCopyFile (@lines)
                    {
                        $count++;
                        
                        next if ($RefCopyFile =~ /^\s*$/);# Skip if it's a blank line

                        # Check if copy and rename: newname,oldname,flags....
                        # oldname is the name on the media

                        ($RefCopyFile, $MediaFile) = split(/,/,$RefCopyFile);

                        $RefCopyFile =~ s/^\s*//i; # Kill leading spaces
                        $RefCopyFile =~ s/\s*$//i; # Kill trailing spaces
                        $MediaFile   =~ s/^\s*//i; # Kill leading spaces
                        $MediaFile   =~ s/\s*$//i; # Kill trailing spaces

                        CheckFile($RefCopyFile, ($MediaFile ne "") ? $MediaFile : $RefCopyFile,
                                  $Current_INF_File->getIndex($RefSection,$count-1));
                    }
                }
                else   ## Not defined in current INF
                {
                    # Note: ChkINF also issues error if not defined *anywhere*
                    if (   ($NTPrintINF_Included)
                        && (NTPrintSectionDefined($RefSection)))
                    {
                        # 8509 => ["Do not reference section '%s' directly, use INCLUDE=NTPrint.inf & appropriate NEEDS=."],
                        AddPrinterWarning(8509, $linenum, $RefSection);
                    }
                    else
                    {
                        # 8543 => ["Install section [%s] not defined in this INF."],
                        AddPrinterError(8543, $linenum, $RefSection);
                    }
                }
            }
        }
    }
    return(1);
}

#-------------------------------------------------------------------------------------------------#
# CheckFile verifies the referenced file is allowed and not in conflict with files that           #
#   ship with the OS (as listed in NTPrint.inf).  Also verifies the file is listed in the         #
#   SourceDisksFiles section of the original INF (the file *really* exists).                      #
#-- CheckFile($CopyFile, $MediaFile, $linenum) ---------------------------------------------------#
sub CheckFile
{
    my $CopyFile  = shift;  # the 'real' name
    my $MediaFile = shift;  # the nane on the media (typically 'compressed' name or same as 'real' name)
    my $linenum   = shift;

    #    verify the CopyFile is not on the verbotten list
    #    verify in ORIGINAL SDFiles
    #    verify in NOT in NTPRINT's SDFiles
    if ((grep(/^$CopyFile$/i, @NoReferencesAllowed)) > 0)
    {
        # 8540 => ["Do not copy file %s directly, use INCLUDE=NTPrint.inf & appropriate NEEDS=."],
        AddPrinterError(8540, $linenum, uc $CopyFile)
            unless (ChkInf::GetGlobalValue("MS_INTERNAL"));
    }
    elsif ((grep(/^$CopyFile$/i, @AllowReferences)) > 0)
    {
        if (!ChkInf::GetGlobalValue("MS_INTERNAL"))
        {
            if (IsDefinedSourceDisksFile($MediaFile, 0)) # SDF section need not exist
            {
                # 8545 => ["File %s ships with the OS.  Do not list it in the [SourceDisksFiles] section(s) of this INF.  Use %s instead."],
                AddPrinterError(8545, $linenum, uc $CopyFile, 
                    (IsDefinedNTPrintSourceDisksFile($CopyFile))
                    ? "INCLUDE=NTPrint.INF"
                    : (IsDefinedLayoutSourceDisksFile($CopyFile))
                      ? "LAYOUTFILE=LayOut.INF"
                      : "[**ChkInf::Printer.pm AllowReferences error**]");
            }
            else
            {
                # 8546 => ["File %s ships with the OS.  Use %s to reference it."],
                AddPrinterError(8546, $linenum, uc $CopyFile, 
                    (IsDefinedNTPrintSourceDisksFile($CopyFile))
                    ? "INCLUDE=NTPrint.INF"
                    : (IsDefinedLayoutSourceDisksFile($CopyFile))
                      ? "LAYOUTFILE=LayOut.INF"
                      : "[**ChkInf::Printer.pm AllowReferences error**]");
            }
        }
    }
    else
    {
        # Check for systemfiles - don't want OEM to overwrite system files
        #   except for those on the redistributable list.
        if ((grep(/^$CopyFile$/i, @RedistributableFiles)) <= 0)
        {
            # Always check for this error (regardless if NTPrint.inf is included)
            if (IsDefinedNTPrintSourceDisksFile($CopyFile))
            {
                # 8542 => ["File %s conflicts with file shipped in the OS (referenced in %s)."],
                AddPrinterError(8542, $linenum, uc $CopyFile, "NTPrint.inf")
                  unless (ChkInf::GetGlobalValue("MS_INTERNAL"));
            }
            elsif (IsDefinedLayoutSourceDisksFile($CopyFile))
            {
                # 8542 => ["File %s conflicts with file shipped in the OS (referenced in %s)."],
                AddPrinterError(8542, $linenum, uc $CopyFile, "Layout.inf");
            }
            else
            {
                # 8541 => ["File %s is not listed in the [SourceDisksFiles] section(s) of this INF."],
                AddPrinterError(8541, $linenum, uc $CopyFile)
                    unless (IsDefinedSourceDisksFile($MediaFile, 1)); # SDF section must exist
            }
        }
        else  # file is redistributable
        {
            # 8541 => ["File %s is not listed in the [SourceDisksFiles] section(s) of this INF."],
            AddPrinterError(8541, $linenum, uc $CopyFile)
                unless (IsDefinedSourceDisksFile($MediaFile, 1)); # SDF section must exist
        }
    }
}

#-------------------------------------------------------------------------------------------------#
# CheckDataSection verifies the contents of the designated DATASECTION.                           #
#   Returns assoc array of Directives found and not found.                                        #
#-- CheckDataSection($Section) -------------------------------------------------------------------#
sub CheckDataSection
{
    my $Section     = shift;
    my @lines       = $Current_INF_File->getSection($Section);
    my $count       = 0;
    my $line;
    my $Directive;
    my $Value;

    my %ValidDirectives = ( "CONFIGFILE"        => 0,  # optional
                            "DATAFILE"          => 0,  # optional (well, maybe - can be the section name)
                            "DRIVERFILE"        => 0,  # optional
                            "DEFAULTDATATYPE"   => 0,  # optional
                          # "EXCLUDEFROMSELECT" => 0,  #          put in [ControlFlags] section, not here
                            "HELPFILE"          => 0,  # optional
                            "LANGUAGEMONITOR"   => 0,  # optional
                            "PRINTPROCESSOR"    => 0,  # optional
                            "VENDORSETUP"       => 0,  # optional
                          );

    print(STDERR "\tInvoking Printer::CheckDataSection ($Section)...\n") if ($DEBUG);
#    if (ChkInf::IsMarked($Section))
#    {
#        print(STDERR "Already processed\n") if ($DEBUG);
#    }
#    else
    {
        ChkInf::Mark($Section);
    
        foreach $line (@lines)
        {
            $count++, next if ($line =~ /^\s*$/);# Skip if it's a blank line
    
            ($Directive, $Value) = ChkInf::SplitLine($Section,$count,$line);

            $Directive = uc $Directive;
    
            if (defined $ValidDirectives{$Directive})
            {
                $ValidDirectives{$Directive}++;

                # 8526 => ["DataSection directive %s may only occur once."],
                AddPrinterError(8526, $Current_INF_File->getIndex($Section,$count), $Directive)
                    if ($ValidDirectives{$Directive} > 1);
            }

            VerifyDirectiveFormat ($Directive, $Value, $Current_INF_File->getIndex($Section,$count));

            $count++;
        }
    
        print(STDERR "\t         Printer::CheckDataSection ($Section)... Done\n") if ($DEBUG > 1);
    }

    return (%ValidDirectives);
}

#-------------------------------------------------------------------------------------------------#
# VerifyDirectiveFormat - Verifies the Directive is properly formatted.  Also verifies the        #
#   directives values if appropriate.                                                             #
#   ALways returns 1                                                                              #
#-- VerifyDirectiveFormat($DirectiveName, $DirectiveValue, $linenum) -----------------------------#
sub VerifyDirectiveFormat
{
    my $Directive = uc shift;
    my $Value     = shift;
    my $linenum   = shift;
    my @Values;
             
    # @Values = ChkInf::GetList($Value,$Current_INF_File->getIndex($Section,$count)); ## strips ALL spaces too!
    $Value =~ s/^\s//; # strip leading spaces
    $Value =~ s/\s$//; # strip trailing spaces

    if ($Directive eq "LANGUAGEMONITOR")
    {
        # format is LanguageMonitor="MonitorName, MonitorDLLName.dll"
        #        or LanguageMonitor="MonitorName", MonitorDLLName.dll
        #        or LanguageMonitor=MonitorName, MonitorDLLName.dll
        # MonitorName may contain spaces only if quoted.
        # DLL name may not contain spaces
        # Only one comma allowed.
        # SplitLine striped the quotes, so must allow spaces in xxx
        unless (   (($Value =~ s/,/,/g) == 1)
                && (   ($Value =~ /^\"?[^\s\"][^\"]*,\s*[^\s\"]*\.DLL\"?$/i)  # matches "xxx,yyy.dll" or xxx,yyy.dll
                    || ($Value =~ /^\"[^\s\"][^\"]*\"\s*,\s*[^\s\"]*\.DLL$/i) # matches "xxx",yyy.dll
                  # || ($Value =~ /^[^\s\"]*\s*,\s*[^\s\"]*\.DLL$/i)          ### matches xxx,yyy.dll  (no spaces in xxx)
                ))
        {
            # 8529 => ["Invalid format.  Should be of the form: %s"],
            AddPrinterError(8529, $linenum, "LanguageMonitor=\"MonitorName, MonitorDLLName.DLL\"");
        }
        else
        {
            $Value  =~ s/\"//g;   # strip the quotes (if any)
            @Values = split(/\s*,\s*/, $Value);

            if (   (!IsDefinedSourceDisksFile($Values[1], 1)) # SDF section must exist
                && (!$NTPrintINF_Included || !IsDefinedNTPrintSourceDisksFile($Values[1]))
                && (!$LayoutINF_Included  || !IsDefinedLayoutSourceDisksFile($Values[1])))
            {
                # 8530 => ["%s \"%s\" is not found in any SourceDisksFiles section."],
                AddPrinterError(8530, $linenum, "MonitorDLLName", $Values[1]);
            }
        }
    }
    elsif ($Directive eq "PRINTPROCESSOR")
    {
        # format is PrintProcessor="PrintProcName, PrintProcDLLName.dll"
        #        or PrintProcessor="PrintProcName", PrintProcDLLName.dll
        #        or PrintProcessor=PrintProcName, PrintProcDLLName.dll
        # PrintProcName may contain spaces only if quoted
        # DLL name may not contain spaces
        # Only one comma allowed.
        # SplitLine striped the quotes, so must allow spaces in xxx
        unless (   (($Value =~ s/,/,/g) == 1)
                && (   ($Value =~ /^\"?[^\s\"][^\"]*,\s*[^\s\"]*\.DLL\"?$/i)  # matches "xxx,yyy.dll" or xxx,yyy.dll
                    || ($Value =~ /^\"[^\s\"][^\"]*\"\s*,\s*[^\s\"]*\.DLL$/i) # matches "xxx",yyy.dll
                  # || ($Value =~ /^[^\s\"]*\s*,\s*[^\s\"]*\.DLL$/i)          ### matches xxx,yyy.dll  (no spaces in xxx)
                ))
        {
            # 8529 => ["Invalid format.  Should be of the form: %s"],
            AddPrinterError(8529, $linenum, "PrintProcessor=\"PrintProcName, PrintProcDLLName.DLL\"");
        }
        else
        {
            $Value  =~  s/\"//g;   # strip the quotes (if any)
            @Values = split(/\s*,\s*/, $Value);
    
            if (!IsDefinedSourceDisksFile($Values[1], 1)) # SDF section must exist
            {
                # 8531 => ["%s \"%s\" is not found in a SourceDisksFiles section in this INF."],
                AddPrinterError(8531, $linenum, "PrintProcDLLName", $Values[1])
            }
        }
    }
    elsif ($Directive eq "VENDORSETUP")
    {
        # format is VendorSetup=DLLFileName, FunctionName   [note, no spaces in either name]
        # DLL name may not contain spaces
        # FunctionName may not contain spaces
        # Only one comma allowed.
        unless (   (($Value =~ s/,/,/g) == 1)
                && ( $Value =~ /^\"?[^\s\"]*\.DLL\s*,\s*[^\s\"]*\"?$/i))  # matches "xxx.dll,yyy" or xxx.dll,yyy
        {
            # 8529 => ["Invalid format.  Should be of the form: %s"],
            AddPrinterError(8529, $linenum, "VendorSetup=DLLFileName.DLL, FunctionName")
        }
        else
        {
            $Value  =~  s/\"//g;   # strip the quotes (if any)
            @Values = split(/\s*,\s*/, $Value);
    
            if (!IsDefinedSourceDisksFile($Values[0], 1)) # SDF section must exist
            {
                # 8531 => ["%s \"%s\" is not found in a SourceDisksFiles section in this INF."],
                AddPrinterError(8531, $linenum, "DLLFileName", $Values[0])
            }
        }
    }
    return 1;
}

#-------------------------------------------------------------------------------------------------#
# IsDefinedSourceDisksFile - Is the supplied name listed in a SourceDisksFiles section of the     #
#   original INF.                                                                                 #
#   Returns > 0 if in section                                                                     #
#          == 0 if not in section and SDF section exists                                          #
#          value of second (optional) parameter if SDF section does not exist (default: 0)        #
#-- IsDefinedSourceDisksFile($Name, $ReturnValueIfNoSDFsection) ----------------------------------#
sub IsDefinedSourceDisksFile
{
    my $Name = shift;
    my $ReturnValueIfNoSDFsection = shift; # optional, defaults to 0

    # Only get list on first use (per INF processed)
    if ($#SDFiles < 0) # -1 if never initialized
    {
        @SDFiles = ChkInf::GetOrigSourceDisksFilesList();
        # foreach (@SDFiles) { print (STDERR "SDFile: $_\n"); }
    }

    if (scalar(@SDFiles > 0))
    {
        # Attempt to avoid substring matches by matching beginline to '='
        return ((grep(/^$Name\s*=/i, @SDFiles)) > 0);
    }
    elsif (defined $ReturnValueIfNoSDFsection)
    {
        return $ReturnValueIfNoSDFsection;
    }
    return 0;
}

#-------------------------------------------------------------------------------------------------#
# IsDefinedNTPrintSourceDisksFile - Is the supplied name listed in a SourceDisksFiles section of  #
#   NTPrint.inf.                                                                                  #
#   Returns boolean                                                                               #
#-- IsDefinedNTPrintSourceDisksFile($Name) -------------------------------------------------------#
sub IsDefinedNTPrintSourceDisksFile
{
    my $Name = shift;

    if (scalar(@NTPrintSDFiles) > 0)
    {
        # Attempt to avoid substring matches by matching beginline to '='
        return ((grep(/^$Name\s*=/i, @NTPrintSDFiles)) > 0);
    }
    return 0;
}

#-------------------------------------------------------------------------------------------------#
# IsDefinedLayoutSourceDisksFile - Is the supplied name listed in a SourceDisksFiles section of   #
#   Layout.inf.                                                                                   #
#   Returns boolean                                                                               #
#-- IsDefinedLayoutSourceDisksFile($Name) --------------------------------------------------------#
sub IsDefinedLayoutSourceDisksFile
{
    #  return 0;  # uncomment this if Layout.inf files should not be checked. 
    my $Name = shift;

    if (scalar(@LayoutSDFiles) > 0)
    {
        # Attempt to avoid substring matches by matching beginline to '='
        return ((grep(/^$Name\s*=/i, @LayoutSDFiles)) > 0);
    }
    return 0;
}

#-------------------------------------------------------------------------------------------------#
# CheckPreviousNames - checks the Previous Names section for validity                             #
#-- CheckPreviousNames($Section) -----------------------------------------------------------------#
sub CheckPreviousNames
{
    my $Section = shift;
    print(STDERR "\tInvoking Printer::CheckPreviousNames ($Section)...\n") if ($DEBUG);
    ChkInf::Mark($Section);

    my @lines       = $Current_INF_File->getOrigSection($Section);
    my $count       = 0;
    my $line;
    my $ModelName;
    my @PrevNames;
    my $PrevName;

    # Format:  Current name = oldname[, oldname[, ... ] ]
    
    # Ensure ModelName is defined in the current INF
    # Ensure PrevNames is *not* defined as a current name in current INF or NTPrint.INF

    foreach $line (@lines)
    {
        $count++, next if ($line =~ /^\s*$/); # Skip if it's a blank line
    
        ($ModelName,$PrevName) = ChkInf::SplitLine($Section,$count,$line);

        # 8536 => ["\"%s\" is not a model name listed in this INF."],
        AddPrinterError(8536, $Current_INF_File->getIndex($Section,$count), $ModelName) 
            unless (exists($CurrentINFModelNames{uc $ModelName}));

        # 8537 => ["Invalid format.  No previous names listed."],
        AddPrinterError(8537, $Current_INF_File->getIndex($Section,$count))
            if ($PrevName =~ /^[\s,]*$/); # all whitespace and commas

        @PrevNames = ChkInf::GetList($PrevName,$Current_INF_File->getIndex($Section,$count));

        foreach $PrevName (@PrevNames)
        {
            $PrevName =~ s/^\s*\"*([^\"]*)\"*\s*$/$1/; # Kill leading/trailing spaces and quotes

            # 8538 => ["Invalid format.  Missing previous name entry (extra commas?)."],
            AddPrinterError(8538, $Current_INF_File->getIndex($Section,$count))
                if ($PrevName eq "");

            if (uc $ModelName eq uc $PrevName) # case insensitive compare
            {
                # 8513 => ["Model name \"%s\" and Previous name \"%s\" are the same%s."],
                AddPrinterWarning(8513, $Current_INF_File->getIndex($Section,$count),
                    $ModelName, $PrevName,
                    ($ModelName eq $PrevName) ? "" : " (differ only in captialization)");
            }
            elsif (exists($CurrentINFModelNames{uc $PrevName}))
            {
                # 8555 => ["Previous name \"%s\" is an active model name listed in this INF."],
                AddPrinterError(8555, $Current_INF_File->getIndex($Section,$count), $PrevName);
            }
            elsif (NTPrintModelNameExists($PrevName))
            {
                # 8539 => ["Previous name \"%s\" is a currently active model name (referenced in NTPrint.inf)."],
                AddPrinterError(8539, $Current_INF_File->getIndex($Section,$count), $PrevName);
            }
        }
        $count++;
    }

    return (1);
}

#-------------------------------------------------------------------------------------------------#
# CheckOEM_URLs - checks the OEM URLs section for validity                                        #
#-- CheckOEM_URLs($Section) ----------------------------------------------------------------------#
sub CheckOEM_URLs
{
    my $Section = shift;
    print(STDERR "\tInvoking Printer::CheckOEM_URLs ($Section)...\n") if ($DEBUG);
    ChkInf::Mark($Section);
    
    my @lines       = $Current_INF_File->getOrigSection($Section);
    my $count       = 0;
    my $line;
    my %ListedOEMNames;
    my $OEMName;
    my $URL;

    # Format OEM name = URL string
    # Ensure OEM name is defined in the current INF (in Manufacturers section)
    # Ensure no duplicates

    foreach $line (@lines)
    {
        $count++, next if ($line =~ /^\s*$/); # Skip if it's a blank line
    
        ($OEMName,$URL) = ChkInf::SplitLine($Section,$count,$line);

        # 8547 => ["\"%s\" is not an OEM (Manufacturer) name listed in this INF."],
        AddPrinterError(8547, $Current_INF_File->getIndex($Section,$count), $OEMName) 
            unless (exists($CurrentINFManufacturerNames{uc $OEMName}));

        # 8508 => ["\"%s\" listed more than once."],
        AddPrinterWarning(8508, $Current_INF_File->getIndex($Section,$count), $OEMName) 
            if (exists($ListedOEMNames{uc $OEMName}));

        $ListedOEMNames{uc $OEMName} = 1;

        $count++;
    }
    
    return (1);
}

#-------------------------------------------------------------------------------------------------#
# CheckOEMFiles - checks the OEMFiles section for validity                                        #
#-- CheckOEMFiles($Section) ----------------------------------------------------------------------#
sub CheckOEMFiles
{
    my $Section = shift;
    print(STDERR "\tInvoking Printer::CheckOEMFiles ($Section)...\n") if ($DEBUG);
    ChkInf::Mark($Section);

    my @lines       = $Current_INF_File->getOrigSection($Section);
    my $count       = 0;
    my $line;
    my $FileType;
    my $FileName;
    my $DriverFiles = 0;
    my $ConfigFiles = 0;
    
    # Format: 
    #   OEMDriverFilen=xxx.dll    (rendering plugin)
    #   OEMConfigFilen=xxx.dll    (UI plugin)
    # where n = 1, 2, 3 .... - must be contiguous numbering

    # Ensure proper Directives
    # Ensure proper numbering (starts at 1, no leading zeroes)
    # Ensure referenced DLL file exists
    foreach $line (@lines)
    {
        $count++, next if ($line =~ /^\s*$/); # Skip if it's a blank line
    
        ($FileType,$FileName) = ChkInf::SplitLine($Section,$count,$line);

        if ($FileType =~ /^OEMDriverFile\d+\d*$/i)
        {
            $FileType =~ s/^OEMDriverFile//i;
            $DriverFiles++;

            # 8549 => ["%s is improperly numbered, expected %s%d."],
            AddPrinterError(8549, $Current_INF_File->getIndex($Section,$count), "OEMDriverFile".$FileType, "OEMDriverFile", $DriverFiles)
                if (   ($FileType =~ /^0/)
                    || ($FileType != $DriverFiles));
        }
        elsif ($FileType =~ /^OEMConfigFile\d+\d*$/i)
        {
            $FileType =~ s/^OEMConfigFile//i;
            $ConfigFiles++;

            # 8549 => ["%s is improperly numbered, expected %s%d."],
            AddPrinterError(8549, $Current_INF_File->getIndex($Section,$count), "OEMConfigFile".$FileType, "OEMConfigFile", $ConfigFiles)
                if (   ($FileType =~ /^0/)
                    || ($FileType != $ConfigFiles));
        }
        else
        {
            # 8548 => ["\"%s\" is not recognized (not OEMDriverFile[1-9] or OEMConfigFile[1-9])."],
            AddPrinterError(8548, $Current_INF_File->getIndex($Section,$count), $FileType);
            next;
        }

        if ($FileName !~ /\.DLL$/i)
        {
            # 8550 => ["\"%s\" is not a DLL name."],
            AddPrinterError(8550, $Current_INF_File->getIndex($Section,$count), $FileName);
        }
        else
        {
            CheckFile($FileName, $FileName, $Current_INF_File->getIndex($Section,$count));
        }

        $count++;
    }
    
    return (1);
}

#-------------------------------------------------------------------------------------------------#
# CheckDestinationDirs - checks the DestinationDirs section for validity                          #
#-- CheckDestinationDirs($Section) ---------------------------------------------------------------#
sub CheckDestinationDirs
{
    my $Section = shift;
    
    ChkInf::Mark($Section);  # already marked by master ChkINF processing
    # return (1) if (ChkInf::GetGlobalValue("MS_INTERNAL"));

    my $line;
    my @lines   = $Current_INF_File->getOrigSection($Section);
    my $count   = 0;

    my($Directive, $Value);

    print(STDERR "\tInvoking Printer::CheckDestinationDirs...\n") if ($DEBUG);

    foreach $line (@lines)
    {
        $count++, next if ($line =~ /^\s*$/); # Skip if it's a blank line

        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);

        # Should we protect use of any of these?????
# [WinntDirectories] -- from Layout.inf
# 2  = system32
# 9  = system32\spool
# 10 = system32\spool\drivers
# 11 = system32\spool\drivers\w32x86\3
# 12 = system32\spool\prtprocs
# 13 = system32\spool\prtprocs\w32x86
# 18 = system32\spool\drivers\w32x86
# 33 = Web\printers
# 34 = system32\spool\drivers\color
# 46 = Web\printers\images
#
# 66000 = GetPrinterDriverDirectory() - local / remote
# 66001 = GetPrintProcessorDirectory() - local / remote
# 66002 = *local* system32 
# 66003 = GetColorDirectory() - local / remote
# 66004 = printer-type specific ASP files

         if (   ($Value >= 66000)
             && ($Value <= 66004))
        {
         # 8503 => ["Use of DirID %s prevents full remote/point-and-print installation."],
            AddPrinterWarning(8503, $Current_INF_File->getIndex($Section,$count), $Value)
                if ($Value == 66002);
        }
        else
        {
            # 8505 => ["DirID %s=%s is discouraged.  Should be in 66000-66004 range in order to support remote install."],
            AddPrinterWarning(8505, $Current_INF_File->getIndex($Section,$count), $Directive, $Value);
        }

        $count++;
    }
    return (1);
}

#-------------------------------------------------------------------------------------------------#
# BuildInteractiveInstallList - gets the list of values associated with the InteractiveInstall    #
#    directive in the ControlFlags section.  Need this list as we are processing each model so    #
#    that the duplicate PnP HWid detection can ignore duplicates that are also in this list.      #  
#-- BuildInteractiveInstallList() ----------------------------------------------------------------#
sub BuildInteractiveInstallList
{
    print(STDERR "\tInvoking Printer::BuildInteractiveInstallList...\n") if ($DEBUG);
    
    my $Section     = "ControlFlags";
    my @lines       = $Current_INF_File->getOrigSection($Section);
    my $count       = 0;
    my $line;
    my $Directive;
    my $Value;

    foreach $line (@lines)
    {
        next if ($line =~ /^\s*$/); # Skip if it's a blank line

        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);

        if ($Directive =~ /^InteractiveInstall/i)
        {
            @InteractiveInstallPnpIDs = ChkInf::GetList($Value,$Current_INF_File->getIndex($Section,$count));
         }
        
        $count++;
    }
    
    return (1);
}

#-------------------------------------------------------------------------------------------------#
# CheckControlFlags - checks the ControlFlags section for validity                                #
#-- CheckControlFlags($Section) ------------------------------------------------------------------#
sub CheckControlFlags
{
    my $Section = shift;
    print(STDERR "\tInvoking Printer::CheckControlFlags ($Section)...\n") if ($DEBUG);
    ChkInf::Mark($Section);
    
    my @lines       = $Current_INF_File->getOrigSection($Section);
    my $count       = 0;
    my $line;
    my $Directive;
    my $Value;
    my @AllPnpIDs   = map(@$_, values(%AllModels)); # flatten the ID's arrays into a single array for easy grep'ing

    foreach $line (@lines)
    {
        $count++, next if ($line =~ /^\s*$/); # Skip if it's a blank line
    
        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);

        if ($Directive =~ /^ExcludeFromSelect|^InteractiveInstall/i)
        {
            my @pnpIDs;
            my $pnpID;
            my $q_pnpID;
            
            if ($Directive =~ /^InteractiveInstall/i)
            {
                @pnpIDs = @InteractiveInstallPnpIDs; # already have the list - don't bother building it again
            }
            else
            {
                @pnpIDs = ChkInf::GetList($Value,$Current_INF_File->getIndex($Section,$count));
            }

            foreach $pnpID (@pnpIDs)
            {
                $q_pnpID = quotemeta($pnpID);
                # 8551 => ["\"%s\" is not an ID listed in this INF."],
                AddPrinterError(8551, $Current_INF_File->getIndex($Section,$count), $pnpID) 
                    unless (grep(/^$q_pnpID$/i, @AllPnpIDs) > 0);
            }
         }
        
        $count++;
    }
    
    return (1);
}

#-------------------------------------------------------------------------------------------------#
# DumpSectionLines - debug routine - prints to STDERR the lines associated with the section                                                                                #
#-- DumpSectionLines($Section) -------------------------------------------------------------------#
sub DumpSectionLines
{
#    return unless $DEBUG;
    my $Section = shift;
    print(STDERR "\t\t   Section:\n");
    if ($Current_INF_File->OrigSectionDefined($Section))
    {
        my @xlines   = $Current_INF_File->getOrigSection($Section);
        my $xline;
        foreach $xline (@xlines)
        {
            print (STDERR "\t\t\t\"$xline\"\n");
        }
    }
}

#-------------------------------------------------------------------------------------------------#
# DumpFilesWhereSectionFound - debug routine                                                      #
#-- DumpFilesWhereSectionFound($Section) ---------------------------------------------------------#
sub DumpFilesWhereSectionFound
{
#    return unless $DEBUG;
    my $Section     = shift;
    my $NumSections;
    my $idx;
    my $InfFile;

    print(STDERR "\t  $Section found in:");
    if ($Current_INF_File->sectionDefined($Section))
    {
        $NumSections = $Current_INF_File->count($Section);  # zero-based value

        for ($idx=0; $idx <= $NumSections; $idx++ )
        {
            $InfFile = $Current_INF_File->instanceData($Section, $idx, "inffile");
            print(STDERR " $InfFile");
        }
        print(STDERR "\n");
    }
    else
    {
        print(STDERR " --nowhere-- \n");
    }
    return;
}

return(1); # Module must return 1 to let ChkINF know it loaded correctly.

