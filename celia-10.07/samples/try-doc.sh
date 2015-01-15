#! /bin/sh

#############
if test $# = 0
then
        LIST=`ls ref/*/log.txt 2> /dev/null`
else
        for TEST in $*
        do
                LIST=$LIST"ref/`basename $TEST`/log.txt "
        done
fi

echo $LIST

for i in $LIST; do
	FDIR=`dirname $i`
	echo "***********$FDIR/log.html"
        ./doc.sh $i &> $FDIR/log.html
        cp -p -r $FDIR ~/public_html/cinv/doc/log/.
done
