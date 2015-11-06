#! /bin/sh

#############
# Constants
#############
SED_COMMAND="sed --in-place --unbuffered"
DEBUG=

INFILE="pan.spl"
OUTFILE="pan.spl.out"

#############
# Useful functions
#############
USAGE() {
  echo "\nspl_io.sh -[2old|2new|2ptr] input_file [output_file]"
  echo "\t-2old\tfrom new encoding to old encoding"
  echo "\t-2new\tfrom old encoding to new encoding"
  echo "\t-2ptr\tfrom new encoding to ptr-like language"
  echo "\t-2c\tfrom old encoding to C-like language"
  echo "\t-2spl\tfrom ptr-like operations to new encoding in spl"
}


#############
# Processing arguments
#############
while true
do
	case "$1" in
	-2old) TO_OLD=1
		shift
	;;
	-2new) TO_NEW=1
		shift
	;;
	-2ptr) TO_PTR=1
		shift
	;;
	-2c) TO_C=1
		shift
	;;
	-2spl) TO_SPL=1
		shift
	;;
	*) break
	;;
	esac
done

if test $# = 0
then
	USAGE
else
	INFILE=$1
fi

if test \( -f $1 \) 
then
	echo "\tinput $INFILE"
else
	echo "\n\tfile $1 does not exists"
	USAGE
	exit 1
fi

if test $# = 2
then
        OUTFILE=$2
fi
echo "\toutput $OUTFILE"


#############
# from old to new
#############
ONULL="_null"
NNULL="0.0"
ODATAF="_data"
NDATAF="cast_i,n"
ODATAR="_data"
NDATAR="cast_d,0"
ONEXTF="_next"
NNEXTF="cast_f,n"
ONEXTR="_next"
NNEXTR="cast_d,n"
ONEW="_new"
NNEW="sqrt_f,n(1)"
OFREE="_free"
NFREE="sqrt_f,0(0)"


if test $TO_NEW
then
	echo "Translation from old to new encoding"
	cp -f $INFILE $OUTFILE
	$SED_COMMAND --expression "s/_[a-z ]*:[ ]*real[ ]*,//g" $OUTFILE
	$SED_COMMAND --expression "s/$ONULL/$NNULL/g" $OUTFILE
	$SED_COMMAND --expression "s/$ONEW/$NNEW/g" $OUTFILE
	$SED_COMMAND --expression "s/$OFREE/$NFREE/g" $OUTFILE
	$SED_COMMAND --expression "s/\([a-z0-9]*\)[ ]*\*[ ]*$ODATAF/$NDATAF(\1)/g" $OUTFILE
	$SED_COMMAND --expression "s/\([a-z0-9]*\)[ ]*\*[ ]*$ONEXTF/$NNEXTF(\1)/g" $OUTFILE
	$SED_COMMAND --expression "s/= \([^\/]*\)[ ]*\/[ ]*$ODATAR/= $NDATAR(\1)/g" $OUTFILE
	$SED_COMMAND --expression "s/= \([^\/]*\)[ ]*\/[ ]*$ONEXTR/= $NNEXTR(\1)/g" $OUTFILE
fi

#############
# from new to old
#############
PREDEF=""
for i in [ data free len new next null ]  ;
do
	PREDEF=$PREDEF" \_$i:real,"
done

if test $TO_OLD
then
	echo "Translation from new to old encoding"
	cp -f $INFILE $OUTFILE
	$SED_COMMAND --expression "s/var /var $PREDEF/g" $OUTFILE
	$SED_COMMAND --expression "s/$NNULL/$ONULL/g" $OUTFILE
	$SED_COMMAND --expression "s/=[ ]*$ONEW/= $NNEW/g" $OUTFILE
	$SED_COMMAND --expression "s/=[ ]*$OFREE/= $NFREE/g" $OUTFILE
	$SED_COMMAND --expression "s/$NDATAF[ ]*(\([a-z0-9]*\))/\1 \* $ODATAF/g" $OUTFILE
	$SED_COMMAND --expression "s/$NNEXTF[ ]*(\([a-z0-9]*\))/\1 \* $ONEXTF/g" $OUTFILE
	$SED_COMMAND --expression "s/= $NDATAR\([^;]*\)/= \1 * $ODATAR/g" $OUTFILE
	$SED_COMMAND --expression "s/= $NNEXTR\([^;]*\)/= \1 * $ONEXTR/g" $OUTFILE
fi

#############
# from new to ptr
#############
PNULL="NULL"
PDATAF="dataf"
PDATAR="fdata"
PNEXTF="nextf"
PNEXTR="fnext"

if test $TO_PTR
then
	echo "Translation from new encoding to ptr-like programs."
	cp -f $INFILE $OUTFILE
	$SED_COMMAND --expression "s/real/intlist/g" $OUTFILE
	$SED_COMMAND --expression "s/$NNULL/$PNULL/g" $OUTFILE
	$SED_COMMAND --expression "s/=[ ]*$NNEW/= $PNEW/g" $OUTFILE
	$SED_COMMAND --expression "s/$NFREE/$PFREE/g" $OUTFILE
	$SED_COMMAND --expression "s/$NDATAF/$PDATAF/g" $OUTFILE
	$SED_COMMAND --expression "s/$NNEXTF/$PNEXTF/g" $OUTFILE
	$SED_COMMAND --expression "s/=[ ]*$NDATAR\([^;]*\)/=.$PDATAR \1/g" $OUTFILE
	$SED_COMMAND --expression "s/=[ ]*$NNEXTR\([^;]*\)/=.$PNEXTR \1/g" $OUTFILE
fi

#############
# from old to C
#############
CNULL="NULL"
CDATAF="->data"
CDATAR="->data"
CNEXTF="->next"
CNEXTR="->next"
CNEW="(intlist) malloc(sizeof(struct _intlist))"
CFREE="free"

if test $TO_C
then
	echo "Translation from old encoding to C-like programs."
	cp -f $INFILE $OUTFILE
	$SED_COMMAND --expression "s/_[a-z ]*:[ ]*real[ ]*,//g" $OUTFILE
	$SED_COMMAND --expression "1i\#include \"intlist.h\"" $OUTFILE
	$SED_COMMAND --expression "s/var /int main() \{/g" $OUTFILE
	$SED_COMMAND --expression "s/begin//g" $OUTFILE
	$SED_COMMAND --expression "s/if /if ( /g" $OUTFILE
	$SED_COMMAND --expression "s/ then/ )  {/g" $OUTFILE
	$SED_COMMAND --expression "s/else/ } else  {/g" $OUTFILE
	$SED_COMMAND --expression "s/endif;/ }/g" $OUTFILE
	$SED_COMMAND --expression "s/while /while ( /g" $OUTFILE
	$SED_COMMAND --expression "s/done;/ }/g" $OUTFILE
	$SED_COMMAND --expression "s/ do/ )  {/g" $OUTFILE
	$SED_COMMAND --expression "s/end/ return 0; }/g" $OUTFILE
	$SED_COMMAND --expression "s/\([_a-zA-Z0-9]*\)[ ]*:[ ]*\([a-z]*\)[ ]*[;,]/\2 \1;/g" $OUTFILE
	$SED_COMMAND --expression "s/real/intlist/g" $OUTFILE
	$SED_COMMAND --expression "s/$ONULL/$CNULL/g" $OUTFILE
	$SED_COMMAND --expression "s/=[ ]*$ONEW/= $CNEW/g" $OUTFILE
	$SED_COMMAND --expression "s/\([a-zA-Z0-9]*\)[ ]*=[ ]*$OFREE/$CFREE(\1)/g" $OUTFILE
	$SED_COMMAND --expression "s/\*[ ]*$ODATAF/$CDATAF/g" $OUTFILE
	$SED_COMMAND --expression "s/\*[ ]*$ONEXTF/$CNEXTF/g" $OUTFILE
	$SED_COMMAND --expression "s/= \([^\/]*\)[ ]*\/[ ]*$ODATAR/$CDATAR = \1/g" $OUTFILE
	$SED_COMMAND --expression "s/= \([^\/]*\)[ ]*\/[ ]*$ONEXTR/$CNEXTR = \1/g" $OUTFILE
fi

#############
# from ptr to new
#############
if test $TO_SPL
then
	echo "Translation from ptr alike programsi to new spl encoding."
	cp -f $INFILE $OUTFILE
	$SED_COMMAND --expression "s/ref/real/g" $OUTFILE
	$SED_COMMAND --expression "s/$PNULL/$NNULL/g" $OUTFILE
	$SED_COMMAND --expression "s/=[ ]*$PNEW/= $NNEW/g" $OUTFILE
	$SED_COMMAND --expression "s/$PFREE/$NFREE/g" $OUTFILE
	$SED_COMMAND --expression "s/$PDATAF/$NDATAF/g" $OUTFILE
	$SED_COMMAND --expression "s/$PNEXTF/$NNEXTF/g" $OUTFILE
	$SED_COMMAND --expression "s/=\.$PDATAR\([^;]*\)/= $NDATAR(\1)/g" $OUTFILE
	$SED_COMMAND --expression "s/=\.$PNEXTR\([^;]*\)/= $NNEXTR(\1)/g" $OUTFILE
fi

#############
# END
#############
echo "\tquit."
