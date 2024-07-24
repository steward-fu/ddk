package SmartGlob;

	use strict;

	#-------------------------------------------------------------------------------------------------
	#  Accepts an array of file spec's & returns an array of every file that list expands to.
	#  Directories in the argument list are interpreted as Dirname\*.*
	#
	# @array SmartGlob(@FileSpecs)
	sub SmartGlob {
	    my (@FileSpecs) = @_;
	    my ($filespec, $dir, $filepattern, @DirectoryFiles,
	    	@MatchedFiles, $file, @AllFiles);

	    foreach $filespec (@FileSpecs) {

			# Account for drives specified w/o the initial '\' (i.e. a:file.inf)
			if ($filespec =~ /^(.:)[^\\]/) {
				$filespec =~ s/^($1)/$1\\/;
			}

	        @MatchedFiles = ();

	        if (($filespec =~ /\~/) and (! -d($filespec)) and (-e($filespec))) {
	            if ($filespec !~ /(.*)\\(.*)/) {
	                $filepattern = $filespec;
	                $dir = ".";
	            } else {
	                $filepattern = $2;
	                $dir = (($1 =~ /^$/) ? "." : $1) ;
	            }
	            push(@MatchedFiles, (($dir =~ /^\.$/) ? ".\\" : $dir."\\") .$filepattern);
	            @AllFiles = sort((@AllFiles, @MatchedFiles));
	            next;
	        }

	        if (!defined($filespec) or $filespec =~ /^$/) {
	            $filespec = ".";
	        }

	        #   is it a directory name ?
	        if ( opendir(LOCALDIR,  $filespec) ) {
	            $dir = $filespec;
	            $filepattern = "*.*";
	        } else  {
	            # extract the directory path
	            # find the last \
	            if ($filespec !~ /(.*)\\(.*)/) {
	                $filepattern = $filespec;
	                $dir = ".";
	            } else {
	                $filepattern = $2;
	                $dir = (($1 =~ /^$/) ? "." : $1) ;
	            }

	            next if (!opendir(LOCALDIR,  $dir));
	        }

			# Turn the pattern into a regex
	        $filepattern = quotemeta($filepattern);
	        $filepattern =~ s/\\\*/.*/g;
	        $filepattern =~ s/\\\?/./g;

	        @DirectoryFiles = readdir LOCALDIR;
	        chop($dir) if ($dir =~ /\\$/);

	        foreach $file (sort @DirectoryFiles) {
	            next if (($file =~ /^\.$/) or ($file =~ /^\.\.$/) or (-d "$dir\\$file"));

	            #! Uncomment to Validate Files as Being 8.3 Compliant!
	            # if ($file !~ /^(.{1,8}\..{0,3})$/) {
	            #     print ("Skipping \"$file\": Not a valid 8.3 name!\n") if ($file =~ /\.inf$/);
	            #     next;
	            # }

	            $file .= "." if ($file !~ /\./);

	            push(@MatchedFiles, (($dir =~ /^\.$/) ? ".\\" : $dir."\\") .$file)
	                if (($file =~ /^$filepattern$/i) &&
	                    (! -d "$dir\\$file") &&
	                    ($file =~ /\.inf$/i));
	        }
	        closedir LOCALDIR;

	        # Concatenate the results
	        @AllFiles = sort((@AllFiles, @MatchedFiles));
	    }
	    my($i);

	    # Remove Duplicates!
	    for ($i=1;$i<=$#AllFiles;$i++) {

	        if ($AllFiles[$i] eq $AllFiles[$i-1]) {
	            splice(@AllFiles, ($i), 1);
	            $i--; # Shortened the array by one, so decrement
	        }
	    }

	    return(@AllFiles);
	}
1; # Return good status
