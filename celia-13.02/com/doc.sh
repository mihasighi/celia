#!/bin/sh
echo "<html><body>"
sed -f $CINV/com/doc.sed $*
echo "</body></html>"

