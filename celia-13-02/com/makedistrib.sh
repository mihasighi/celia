
cd $CINV
make distclean

cd $CINV/..

tar cv --exclude-from=$CINV/com/nodistrib.txt -f cinv-0.3.tar -z cinv/

