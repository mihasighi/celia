#!/bin/sh

SED_COMMAND="sed --in-place --unbuffered"
WEB=$1

$SED_COMMAND --expression "s/\/\*Loc=/\/\*File:/g" $WEB/index.html
$SED_COMMAND --expression "s/\.\.\/\.\.\/\.\.\/c\// /g" $WEB/index.html
$SED_COMMAND --expression "s/[:]*([0-9]*,[\-]*[0-9]*)//g" $WEB/index.html

