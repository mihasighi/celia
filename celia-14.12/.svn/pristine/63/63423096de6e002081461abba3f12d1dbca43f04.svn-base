#! /bin/sh

VERSION=old
OUTPUT=c
REFDIR=ref$VERSION

#############
if test $# = 0
then
        LIST=`ls $REFDIR/*/log.txt 2> /dev/null`
else
        for TEST in $*
        do
                LIST=$LIST"$REFDIR/`basename $TEST`/log.txt "
        done
fi

echo $LIST

for i in $LIST; do
	FDIR=`dirname $i`
	echo "***********$FDIR/log.html"
        ./spl_io.sh -2$OUTPUT $FDIR/$i $FDIR/$i.out
        ./doc.sh $i.out &> $FDIR/log.html
        cp -p -r $FDIR ~/public_html/cinv/doc/log/.
done
