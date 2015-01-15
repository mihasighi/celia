
cd $CINV
make clean

cd $CINV/..

tar cv --exclude-from=$CINV/com/nodistrib.txt -f cinv-0.2.2.tar -z cinv/

