#! /bin/sh


if test $# = 0
then
        LIST=`ls *.c 2> /dev/null`
else
        for TEST in $*
        do
                LIST=$LIST"`basename $TEST ` "
        done
fi

for i in $LIST; do
        FNAME=`basename $i .c`
	frama-c-Celia.byte -celia $i 1> log/$FNAME
	mv -f pan.clim clim/$FNAME
done

