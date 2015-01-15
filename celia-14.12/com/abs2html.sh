#! /bin/sh

#############
# Constants
#############
SED_COMMAND="sed --in-place --unbuffered"
DEBUG=

INFILE=$1
OUTFILE="pan.abs.html"

#############
# Useful functions
#############
USAGE() {
  echo "\nabs2html.sh input_file.abs [output_file]"
}

#############
# Translation
#############
AMS="\<a target=\"_blank\" href=\"mset\/sid_"
AET="\.png\"\>mset\<\/a\>, "
ALS="\<a target=\"_blank\" href=\"lsum\/sid_"
AUM="\.png\"\>lsum\<\/a\>, "
AUC="<a target=\"_blank\" href=\"ucons\/sid_"
AONS="\.png\"\>ucons\<\/a\>, "
ASTART="<a target=\"_blank\" href=\"ucons\/sid_"
APNG="\.png\"\>"
AEND="\<\/a\>"

	echo "Translation from abs file to html file."
	cp -f $INFILE $OUTFILE
	$SED_COMMAND --expression "s/analysis: /analysis: \n/g" $OUTFILE
	$SED_COMMAND --expression "s/\[|$/\[|0>=0|\]/g" $OUTFILE
	$SED_COMMAND --expression "3d" $OUTFILE
	$SED_COMMAND --expression "s/:(\([0-9][0-9]*\),\([0-9][0-9]*\)): \[|\([0-9][0-9]*\)>=0|\]/:($ASTART\2$APNG sid:\2 $AEND)/g" $OUTFILE
#	$SED_COMMAND --expression "s/:(\([0-9][0-9]*\),\([0-9][0-9]*\)): \[|\([0-9][0-9]*\)>=0|\]/:(sid:\2): $AMS\2$AET$ALS\2$AUM$AUC\2$AONS/g" $OUTFILE
	$SED_COMMAND --expression "s/:(\([0-9][0-9]*\),\([0-9][0-9]*\)): \([a-z][a-z]*\)/:($ASTART\2$APNG sid:\2 $AEND)/g" $OUTFILE
#	$SED_COMMAND --expression "s/:(\([0-9][0-9]*\),\([0-9][0-9]*\)): \([a-z][a-z]*\)/:(sid:\2): \3/g" $OUTFILE
	$SED_COMMAND --expression "s/:(\([0-9][0-9]*\),\-\([0-9][0-9]*\)): \[|\([0-9][0-9]*\)>=0|\]/:($ASTART\1_\2$APNG sid:\1_\2 $AEND)/g" $OUTFILE
#	$SED_COMMAND --expression "s/:(\([0-9][0-9]*\),\-\([0-9][0-9]*\)): \[|\([0-9][0-9]*\)>=0|\]/:(sid:\1_\2): $AMS\1_\2$AET$ALS\1_\2$AUM$AUC\1_\2$AONS/g" $OUTFILE
	$SED_COMMAND --expression "s/:(\([0-9][0-9]*\),\-\([0-9][0-9]*\)): \([a-z][a-z]*\)/:($ASTART\1_\2$APNG sid:\1_\2 $AEND)/g" $OUTFILE
#	$SED_COMMAND --expression "s/:(\([0-9][0-9]*\),\-\([0-9][0-9]*\)): \([a-z][a-z]*\)/:(sid:\1_\2): \3/g" $OUTFILE
	$SED_COMMAND --expression "s/^/\<br\>/g" $OUTFILE
	$SED_COMMAND --expression "s/\.\.\/\.\.\/\.\.\/c\// /g" $OUTFILE

#############
# END
#############
echo "quit."
