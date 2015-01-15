#! /bin/sh

#############
#MEMCHECK="valgrind --tool=memcheck --verbose --leak-check=yes --show-reachable=yes "
#MEMCHECK="../../../tools-other/valgrind-3.5.0/bin/valgrind --tool=memcheck --verbose --leak-check=yes --show-reachable=yes  --dsymutil=yes "
#MEMCHECK=memtime 
MEMCHECK="time --verbose "

#############
#DEBUG="_debug"
DEBUG=

#############
if test $# = 0
then
        LIST=`ls src/*.spl 2> /dev/null`
else
        for TEST in $*
        do
                LIST=$LIST"src/`basename $TEST ` "
        done
fi

for i in $LIST; do
	FNAME=`basename $i .spl`
	echo "***********$FNAME"
	for j in `ls src/$FNAME-??.txt`; do
		CF=`basename $j .txt`
		echo "====$CF"
		NR=${CF##$FNAME}
		echo "====$NR"
		cp -f $j cinv.txt

		echo "ooo./cinv-lsumprd on $i"
#		$MEMCHECK ./cinv-lsumprd$DEBUG.opt -debug 2 -domain shape $i 
		echo "eee./cinv-lsumprd on $i"
		$MEMCHECK ./cinv-lsumprd$DEBUG.opt -domain shape $i &> log.txt
		LOG=ref/$FNAME-lsum-prd$NR
		if test \( ! -d $LOG \)
		then
			mkdir $LOG
		fi
        	mv f_*.shp log.txt $LOG/
#		diff $LOG/log.txt ref/$FNAME-lsum-prd$NR/log.txt

		echo "ooo./cinv-lsumrel on $i"
#		$MEMCHECK ./cinv-lsumrel$DEBUG.opt -debug 2 -domain shape $i
		echo "eee./cinv-lsumrel on $i"
		$MEMCHECK ./cinv-lsumrel$DEBUG.opt -domain shape $i &> log.txt
		LOG=ref/$FNAME-lsum-rel$NR
		if test \( ! -d $LOG \)
		then
			mkdir $LOG
		fi
        	mv f_*.shp log.txt $LOG/
#		diff $LOG/log.txt ref/$FNAME-lsum-rel$NR/log.txt
	done
done
