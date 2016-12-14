#!/bin/bash

adb remount

# push iwlist
adb push iwlist /system/bin/

# push pictures
for i in `ls *.png`
do
    adb push $i /res/images/
done

# push sound files
for i in `ls *.wav`
do
    adb push $i /data/local/tmp/
done
