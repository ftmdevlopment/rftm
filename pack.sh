#!/bin/bash

if [ -e ftmres ]; then
	echo clear last package ...
	rm -rf ftmres
fi

if [ -e MD5SUM.txt ]; then
    echo clear last MD5SUM ...
    rm MD5SUM.txt
fi

mkdir ftmres

TARGET_BIN=libs/arm64-v8a/rftm

echo build if need ...
if [ ! -e $TARGET_BIN ]; then
    ndk-build
fi
echo copy binary ...
cp -v $TARGET_BIN ftmres/

echo copy resources ...
for i in `ls res/`; do
    sleep 1  # make different timestamp for sorting by time
    cp -rv res/$i ftmres/
done

echo calculate md5 ...
files=`tree -ifpt ftmres/  | grep '\[-' | cut -d' ' -f3`
for f in $files; do
    md5sum $f >> MD5SUM.txt
done

echo show md5 ...
cat MD5SUM.txt

#TIMESTAMP=`date +%Y%m%d-%H%M%S`
TIMESTAMP=`ls -l --time-style=+%Y%m%d-%H%M%S $TARGET_BIN | cut -d' ' -f6`
PACKAGE=FTM-$TIMESTAMP.tar.gz
echo packaging $PACKAGE ...
tar czvf $PACKAGE MD5SUM.txt ftmres/
