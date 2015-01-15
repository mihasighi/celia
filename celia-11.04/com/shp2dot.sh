#!/bin/sh

DOTTY=
if test $1 = "-show" 
then
	DOTTY=1
	shift
fi

sed -f $CINV/com/shp2dot.sed $* 1> $*.dot

if test $DOTTY
then
	dotty $FILEN.dot
fi
