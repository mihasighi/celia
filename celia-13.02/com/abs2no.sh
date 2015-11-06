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

	echo "Translation from abs file to png file."
	cp -f $INFILE $OUTFILE
	$SED_COMMAND --expression "s/:(\([0-9][0-9]*\),\([0-9][0-9\_]*\)): \[|\([0-9][0-9]*\)>=0|\]/:\2:\3/g" $OUTFILE
        $SED_COMMAND --expression "s/:(\([0-9][0-9]*\),\([0-9][0-9]*\)): \([a-z][a-z]*\)/:\2:\3/g" $OUTFILE
        $SED_COMMAND --expression "s/:(\([0-9][0-9]*\),\-\([0-9][0-9]*\)): \[|\([0-9][0-9]*\)>=0|\]/:\1_\2:\3/g" $OUTFILE
        $SED_COMMAND --expression "s/:(\([0-9][0-9]*\),\-\([0-9][0-9]*\)): \([a-z][a-z]*\)/:\1_\2:\3/g" $OUTFILE

	$SED_COMMAND --expression "s/[a-z0-9\-\_\.\/A-Z]+/ /g" $OUTFILE

#############
# END
#############
echo "quit."
