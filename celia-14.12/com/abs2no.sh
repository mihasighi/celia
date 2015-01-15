#! /bin/sh

#############
# Constants
#############
SED_COMMAND="sed --in-place --unbuffered"
DEBUG=

INFILE="pan.abs"
OUTFILE="pan.abs.png"

#############
# Useful functions
#############
USAGE() {
  echo "\nabs2html.sh input_file.abs [output_file]"
}

#############
# Translation
#############

	echo "Translation from abs file to png file..."
	cp -f $INFILE $OUTFILE
	$SED_COMMAND --expression "1,1s/analysis: /analysis: \n/g" $OUTFILE
	$SED_COMMAND --expression "1d" $OUTFILE
	$SED_COMMAND --expression "s/\[|$/\[|0>=0|\]/g" $OUTFILE
	$SED_COMMAND --expression "2d" $OUTFILE
	$SED_COMMAND --expression "s/:(\([0-9][0-9]*\),\([0-9][0-9\_]*\)): \[|[\n ]*\([0-9][0-9]*\)>=0|\]/:\2:\3/g" $OUTFILE
        $SED_COMMAND --expression "s/:(\([0-9][0-9]*\),\([0-9][0-9]*\)): \([a-z][a-z]*\)/:\2:\3/g" $OUTFILE
        $SED_COMMAND --expression "s/:(\([0-9][0-9]*\),\-\([0-9][0-9]*\)): \[|\([0-9][0-9]*\)>=0|\]/:\1_\2:\3/g" $OUTFILE
        $SED_COMMAND --expression "s/:(\([0-9][0-9]*\),\-\([0-9][0-9]*\)): \([a-z][a-z]*\)/:\1_\2:\3/g" $OUTFILE

	$SED_COMMAND --expression "s/[a-z0-9\-\_\.\/A-Z]+/ /g" $OUTFILE

#############
# END
#############
echo "done."
