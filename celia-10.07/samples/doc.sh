#!/bin/sh
echo "<html><body>"
sed -f doc.sed $*
echo "</body></html>"

