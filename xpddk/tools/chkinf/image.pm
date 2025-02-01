# Copyright 1999-2000 Microsoft Corporation

# The name of this module. For our purposes it must be all in caps.
package IMAGE;

    use strict;          # Keep the code clean
    my $DEBUG  = 0;      # Set to 1 for debugging output

    #---------------------------------------------------#
    # Module level variables
    #---------------------------------------------------#
    my $Version= "5.1.2402.0"; # Version of this file
    my $Current_INF_File ;     # Stores name of the current INF file.
    my $Current_HTM_File ;     # Name of the output file to use.

    my(@Device_Class_Options); # Array to store device specific options.

    my(%AllModels);            # Hash of all models present in INF

    my(%ErrorTable) = (1999 => [" ERROR NUMS < 2000 ARE RESERVED "],
                       2001 => ["The value of %s should be %s."],
                       2002 => ["Section [%s] could not be found."],
                       2003 => ["%s is expected to %s."],
                       2004 => ["Too many values for directive %s"],
                       2005 => ["Unrecognized directive: %s"],
                       9999 => [" DO NOT USE THIS ERROR NUM "]);

    my(%WarningTable)=(1999 => [" WARNING NUMS < 2000 ARE RESERVED "],
                       3001 => ["\"VendorSetup\" may be included it for backward compatibility with Windows 98, but it has no effect on Windows 2000."],
                       9999 => [" DO NOT USE THIS WARNING NUM "]);

    #--- Helper Subroutines ---#
    sub TRUE  { return(1); }
    sub FALSE { return(0); }


#-------------------------------------------------------------------------------------------------#
#-- CheckModel($Model) ---------------------------------------------------------------------------#
sub CheckModel {

    my $line;
    my $Section  =  shift;
    my @lines    =  $Current_INF_File->getSection($Section);
    my $count    =  0;
    my $Model    = $_[0];
       $Model    = uc($Model);

    my($Directive, @Values, $Value, $Temp);

    # Only need to check device specific directives. ChkInf already checked universal ones!
    my(%ImageDirectives) = ('SUBCLASS'         => "StillImage",
                            'DEVICETYPE'       => " ",
                            'CAPABILITIES'     => " ",
                            'EVENTS'           => "EventSectionName",
                            'DEVICEDATA'       => "DeviceDataSectionName",
                            'PROPERTYPAGES'    => " ",
                            'DEVICESUBTYPE'    => " ",
                            'CONNECTION'       => " ",
                            'VENDORSETUP'      => " ",
                            'ICMPROFILES'      => " ",
                            'UNINSTALLSECTION' => "UninstallSectionName");

    print(STDERR "\tEntering CheckModel($Model)...\n") if ($DEBUG);

    foreach $line (@lines) {
        $count++, next if ($line =~ /^\s*$/);# Skip if it's a blank line

        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);

        if (! defined($ImageDirectives{uc $Directive})) {
            # No error - ChkInf.pm should catch it
            $count++;
            next;
        }

        # Once we get here, we know $Directive is valid =-)
        if (uc $Directive eq "SUBCLASS") {
            AddError($Current_INF_File->getIndex($Section,$count), 2001, "Subclass", "StillImage") unless (uc($Value) eq "STILLIMAGE");

        } elsif (uc $Directive eq "DEVICETYPE") {
            AddError($Current_INF_File->getIndex($Section,$count), 2001, "DeviceType", "1,2 or 3") unless (($Value == 1) || ($Value == 2) || ($Value == 3));

        } elsif ($Directive eq "CAPABILITIES") {
            if ($Value =~ /^0x/i) {
                $Value =~ s/^(0X)//;
                $Value = hex($Value);
            }
            AddError($Current_INF_File->getIndex($Section,$count), 2001, "Capabilities", "numeric") unless ($Value =~ /^\d*$/);

        } elsif (uc $Directive eq "EVENTS") {
            @Values = ChkInf::GetList($Value,$Current_INF_File->getIndex($Section, $count));
            foreach $Value (@Values) {
                if ( $Current_INF_File->sectionDefined($Value) ) {
                    VerifyEvents($Value);
                } else {
                    AddError($Current_INF_File->getIndex($Section,$count), 2002, $Value);
                }
            }

        } elsif (uc $Directive eq "DEVICEDATA") {
            @Values = ChkInf::GetList($Value,$Current_INF_File->getIndex($Section, $count));
            foreach $Value (@Values) {
                if ( $Current_INF_File->sectionDefined($Value) ) {
                    VerifyDeviceData($Value);
                } else {
                    AddError($Current_INF_File->getIndex($Section,$count), 2002, $Value);
                }
            }

        } elsif (uc $Directive eq "PROPERTYPAGES") {
            @Values = ChkInf::GetList($Value,$Current_INF_File->getIndex($Section, $count));
            if ($Values[0] !~ /[\.dll|\.cpl]$/i) {
                AddError($Current_INF_File->getIndex($Section,$count), 2003, "Control_Panel_DLL", "end in .dll or .cpl");
            }
            if (defined($Values[3])) {
                AddError($Current_INF_File->getIndex($Section,$count), 2004, "PropertyPages");
            }

        } elsif (uc $Directive eq "DEVICESUBTYPE") {
            if ($Value =~ /^0x/i) {
                $Value =~ s/^(0X)//;
                $Value = hex($Value);
            }
            AddError($Current_INF_File->getIndex($Section,$count), 2001, "DeviceSubType", "numeric") unless ($Value =~ /^\d*$/);

        } elsif (uc $Directive eq "CONNECTION") {
            AddError($Current_INF_File->getIndex($Section,$count), 2001, "Connection", "serial or parallel")
                unless ((uc($Value) eq "SERIAL") or (uc($Value) eq "PARALLEL"));

        } elsif (uc $Directive eq "VENDORSETUP") {
            AddWarning($Current_INF_File->getIndex($Section,$count), 3001);

        } elsif (uc $Directive eq "ICMPROFILES") {
            AddError($Current_INF_File->getIndex($Section,$count), 2001, "ICMProfiles", "&lt;ProfileName.ICM&gt;") unless ($Value =~ /\"?.*\.ICM\"?$/i);

        } elsif (uc $Directive eq "UNINSTALLSECTION") {
            @Values = ChkInf::GetList($Value,$Current_INF_File->getIndex($Section, $count));
            foreach $Value (@Values) {
                if ( $Current_INF_File->sectionDefined($Value) ) {
                    VerifyUninstallSection($Value);
                } else {
                    AddError($Current_INF_File->getIndex($Section,$count), 2002, $Value);
                }
            }

        } else {
            # die("Unexpected error: IMAGE::CheckModule():Directive=$Directive\n");
        }


        $count++;
    }
}

#---------------------------------------------------#
# Required subroutine! Used to verify a Directive as
# being device specific.
#---------------------------------------------------#
sub DCDirective {
    my($Directive) = uc($_[1]);

    my(%ValidDirectives) = ( "SUBCLASS"         => TRUE,
                             "DEVICETYPE"       => TRUE,
                             "CAPABILITIES"     => TRUE,
                             "EVENTS"           => TRUE,
                             "DEVICEDATA"       => TRUE,
                             "PROPERTYPAGES"    => TRUE,
                             "DEVICESUBTYPE"    => TRUE,
                             "CONNECTION"       => TRUE,
                             "VENDORSETUP"      => TRUE,
                             "ICMPROFILES"      => TRUE,
                             "UNINSTALLSECTION" => TRUE);

    return(FALSE) unless (defined($ValidDirectives{$Directive}));
    return(TRUE);
    # Only return True if the directive is valid
}


#---------------------------------------------------#
# Verifies that the module loaded correctly.  Be sure
# to change the string to the name of your module.
#---------------------------------------------------#
sub Exists {
    print(STDERR "Module \"IMAGE\" Loaded\n");
    return(1); # return 1 on success
}


#-------------------------------------------------------------------------------------------------#
#-- ExpandModelSection($line, $Section) ----------------------------------------------------------#
sub ExpandModelSection {
    my $Section  = shift;
    my $line;
    my @lines    = $Current_INF_File->getSection($Section);
    my $count    = 0;

    my(%Models)  =  ("","");
    my($TempVar);

    my($Directive, @Values, $Value);

    print(STDERR "\tInvoking Display::ExpandModelSection...\n") if ($DEBUG);

    foreach $line (@lines) {
        $count++, next if ($line =~ /^\s*$/);# Skip if it's a blank line

        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);
        @Values             = ChkInf::GetList($Value,$Current_INF_File->getIndex($Section,$count));

        $Values[0] =~ s/^\s*//i; # Kill leading spaces
        $Values[0] =~ s/\s*$//i; # Kill trailing spaces

        if (defined($Values[0])) {
            $Values[1] = "x" unless ( defined($Values[1]) );
            $Models{$Values[0]} = $Values[1];
        }
        $count++;
    }
    return(%Models);
}

#---------------------------------------------------#
# The module's main checking routine.
# This is your entry point for verifying an INF
#---------------------------------------------------#
sub InfCheck {

    print("\tBeginning IMAGE::InfCheck(@_)\n") if ($DEBUG);

    unless ( $Current_INF_File->sectionDefined("Manufacturer") ) {
        print(STDERR "\t$Current_INF_File->{inffile} contains no [Manufacturer] section.\n") if ($DEBUG);
        return(FALSE)
    }

    %AllModels  = LearnModels();

    # For every model, check the DDInstall section
    foreach (sort(keys(%AllModels))) {
        warn("\tUndefined model found in array: $_\n"), next if (!defined($AllModels{$_}));
        if ( $Current_INF_File->sectionDefined($_) ) {
            CheckModel($_) unless ($AllModels{$_} eq "DONE");
            $AllModels{$_} = "DONE";
        }
    }
    return(1); # return 1 on success
}


#---------------------------------------------------#
# Is called when module is first included.
# Use it to set your per-inf variables.
#---------------------------------------------------#
sub InitGlobals {
    # First parameter is the name of the current INF.
    $Current_INF_File = $_[1];

    # Output file = htm subdir + ((INF Name - .inf) + .htm)
    $Current_HTM_File = $Current_INF_File->{inffile};
    $Current_HTM_File =~ s/\.inf$/\.htm/i;
    $Current_HTM_File = "htm\\".substr($Current_HTM_File,rindex($Current_HTM_File, "\\")+1);

    # Second parameter is a list of Device Specific options.
    my($DC_Specific_Options) = $_[2];

    # $DC_Specific_Options is a string that contains all device
    #   class options delimited by &'s
    if (defined($DC_Specific_Options)) {
        @Device_Class_Options = split(/&/,$DC_Specific_Options);
    } else {
        @Device_Class_Options = ("NULL","NULL");
    }

    # Array must be shifted since first element also had a & prepended
    shift(@Device_Class_Options);

    return(1); # return 1 on success
}


#-------------------------------------------------------------------------------------------------#
# LearnModels - Returns an array of all models listed in the INF                                  #
#-- LearnModels() --------------------------------------------------------------------------------#
sub LearnModels {
    return(FALSE) if (! $Current_INF_File->sectionDefined("Manufacturer") );

    my $Section = "Manufacturer";
    my $line;
    my @lines   = $Current_INF_File->getSection("Manufacturer");
    my $count   = 0;

    my($Directive, $Value);
    my %Models;

    print(STDERR "\tInvoking Display::ProcManu...\n") if ($DEBUG);

    foreach $line (@lines) {
        $count++, next if ($line =~ /^\s*$/); # Skip if it's a blank line

        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);

        # [Manufacturers] may not have both a directive & a value
        if ($Value =~ /^\s*$/) {
            $Value     = $Directive;
            $Directive = "";
        } else {
            $Value    =~  s/^\"(.*)\"/$1/;   # And Strip the Quotes!
        }

        $Value =~ s/^\s*//; # Kill leading spaces
        $Value =~ s/\s*$//; # Kill trailing spaces

        if ( $Current_INF_File->sectionDefined($Value) ) {
            # Add the models from $INFSections{$Value} to our running list
            %Models = (%Models, ExpandModelSection($Value));
        }
        $count++;
    }
    return(%Models);
}

#---------------------------------------------------#
# Allows to add Device specific information to the
# top of the INF summary page.  The information here
# should be brief and link to detailed summaries
# below. (Which will be written using PrintDetials).
#---------------------------------------------------#
sub PrintHeader {
    return(1); # return 1 on success
}


#---------------------------------------------------#
# Allows addition of device specific results to the
# detail section on the INF summary.
#---------------------------------------------------#
sub PrintDetails {
    return(1); # return 1 on success
}

###################################################################################################
#+-----------------------------------------------------------------------------------------------+#
#|                                   HELPER SUBROUTINES                                          |#
#+-----------------------------------------------------------------------------------------------+#
###################################################################################################

#-------------------------------------------------------------------------------------------------#
# This sub adds a new error to the list of errors for the file.                                   #
#-- AddError($ErrorNum, $LineNum, @ErrorArgs) ----------------------------------------------------#
sub AddError {
    my($LineNum) = $_[0], shift;
    my($ErrorNum)  = $_[0], shift;
    my(@ErrorArgs)= @_;

    $ErrorArgs[0] = " " if (! defined($ErrorArgs[0]));
    my($str, $this_str, $info_err, $format_err);

    $info_err = $ErrorTable{"$ErrorNum"};

    if ( !defined($info_err) ) {
        $this_str = "Unknown error $ErrorNum.";
    } else {
        $format_err = $$info_err[0];
        $this_str = sprintf($format_err, @ErrorArgs);
    }
    ChkInf::AddDeviceSpecificError($LineNum, $ErrorNum, $this_str);
}

#-------------------------------------------------------------------------------------------------#
# This sub adds a new warning to the list of warnings for the file.                               #
#-- AddWarning($WarnNum, $LineNum, @ErrorArgs) ---------------------------------------------------#
sub AddWarning {
    my($LineNum)  = $_[0], shift;
    my($WarnNum)  = $_[0], shift;
    my(@WarnArgs) = @_;

    $WarnArgs[0] = " " if (! defined($WarnArgs[0]));
    my($str, $this_str, $info_wrn, $format_wrn);

    $info_wrn = $WarningTable{"$WarnNum"};

    if ( !defined($info_wrn) ) {
        $this_str = "Unknown warning $WarnNum.";
    } else {
        $format_wrn = $$info_wrn[0];
        $this_str = sprintf($format_wrn, @WarnArgs);
    }
    ChkInf::AddDeviceSpecificWarning($LineNum, $WarnNum, $this_str);
}

#-------------------------------------------------------------------------------------------------#
# Verifies DEVICEDATA Sections                                                                    #
#-- VerifyDeviceData($linenum, $SecName) ---------------------------------------------------------#
sub VerifyDeviceData {
    my $line;
    my $Section =  shift;
    my @lines   = $Current_INF_File->getSection($Section);
    my $count    = 0;

    my($Directive, $Value);

    print(STDERR "\tBeginning VerifyDeviceData...\n") if ($DEBUG);
    foreach $line (@lines) {
        $count++, next if ($line =~ /^\s*$/); # Skip if it's a blank line

        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);
        $count++;
    }

	ChkInf::Mark($Section);

    return(1);
}

#-------------------------------------------------------------------------------------------------#
# Verifies EVENTS Sections                                                                        #
#-- VerifyEvents($linenum, $SecName) -------------------------------------------------------------#
sub VerifyEvents {
    my $line;
    my $Section  =  shift;
    my @lines    = $Current_INF_File->getSection($Section);
    my $count    = 0;

    my($Directive, @Values, $Value);

    print(STDERR "\tBeginning VerifyEvents...\n") if ($DEBUG);
    foreach $line (@lines) {
        $count++, next if ($line =~ /^\s*$/); # Skip if it's a blank line

        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);
        @Values = ChkInf::GetList($Value,$Current_INF_File->getIndex($Section, $count));

        AddError($Current_INF_File->getIndex($Section, $count), 2003, "Directive", "have 3 parameters") unless (defined($Values[2]) );
        $count++;
    }

	ChkInf::Mark($Section);

    return(1);
}

#-------------------------------------------------------------------------------------------------#
# Verifies UNINSTALLSECTION Sections                                                              #
#-- VerifyUninstallSectionEvents($linenum, $SecName) ---------------------------------------------#
sub VerifyUninstallSection {
    my $line;
    my($Section) =  shift;
    my @lines    = $Current_INF_File->getSection($Section);
    my $count    = 0;

    my($Directive, $Value);

    my(%Directives) =  ("DELFILES" => TRUE,
                        "DELREG"   => TRUE);

    foreach $line (@lines) {
        $count++, next if ($line =~ /^\s*$/); # Skip if it's a blank line

        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);

        if (defined $Directives{uc($Directive)}) {
            $Directive = uc($Directive);
            ChkInf::DELFILES($Section,$count,$Value) if ($Directive eq "DELFILES");
            ChkInf::DELREG($Section,$count,$Value)   if ($Directive eq "DELREG");
        } else {
            AddError($Current_INF_File->getIndex($Section,$count), 2005, $Directive);
        }

        $count++;
    }

	ChkInf::Mark($Section);

    return(1);
}

return(1); # Module must return 1 to let chkinf know it loaded correctly.
