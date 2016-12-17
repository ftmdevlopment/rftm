#!/bin/bash

adb remount

# push bins
adb push iwlist /system/bin/
adb push ledtest /system/bin/
adb push camera_test /system/bin/

adb shell mkdir -p /system/ftmres/
adb shell mkdir -p /system/ftmres/img/

# push pictures
for i in `ls *.png`
do
    adb push $i /system/ftmres/img/
done

# push sound files
for i in `ls *.wav`
do
    adb push $i /system/ftmres/
done
