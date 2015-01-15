#!/bin/sh

CINV=$HOME/Parallelisme/tools/cinv
WEB=$HOME/public_html/celia/samples
SED_COMMAND="sed --in-place --unbuffered"

FILENAME=$1

echo "create sample directory\n"
mkdir $WEB/$FILENAME
mkdir $WEB/$FILENAME/mset
mkdir $WEB/$FILENAME/lsum
mkdir $WEB/$FILENAME/ucons
cd $WEB/$FILENAME

echo "create results files (1/3)\n"
cp -p $CINV/samples/ref/$FILENAME/mset/pan-nm.c $WEB/$FILENAME/.
cp -p $CINV/samples/c/$FILENAME.c $WEB/$FILENAME/.

echo "create results files (2/3)\n"
$CINV/com/abs2html.sh $CINV/samples/ref/$FILENAME/mset/pan.abs

echo "create index file\n"
cp $WEB/index-gen1.html index-gen1.html
$SED_COMMAND --expression "s/FILENAME/$FILENAME.c/g" index-gen1.html
cat index-gen1.html $CINV/samples/c/$FILENAME.c $WEB/index-gen2.html $CINV/samples/ref/$FILENAME/mset/pan-nm.c $WEB/index-gen3.html pan.abs.html $WEB/index-gen4.html 1> index.html
rm index-gen1.html
$SED_COMMAND --expression "s/\/\*Loc=/\/\*File:/g" index.html
$SED_COMMAND --expression "s/\.\.\/\.\.\/\.\.\/c\// /g" index.html
$SED_COMMAND --expression "s/[:]*([0-9]*,[\-]*[0-9]*)//g" index.html

echo "create results file (png)\n"
for i in lsum mset ucons; do
  cd $CINV/samples/ref/$FILENAME/$i/
  $CINV/com/abs2no.sh
  $CINV/com/shp2png.pl
  cp -p $CINV/samples/ref/$FILENAME/$i/sid*.png $WEB/$FILENAME/$i/.
done

