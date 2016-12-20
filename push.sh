#!/bin/bash

FTM_RESOUCE_PATH=/system/ftmres/

adb remount

echo ensure $FTM_RESOUCE_PATH exists
adb shell mkdir -p $FTM_RESOUCE_PATH

echo push bins
adb push res/bin/ /system/bin/
#adb shell "chmod +x $FTM_RESOUCE_PATH/bin/*"

echo push pictures
adb push res/img/ $FTM_RESOUCE_PATH

echo push sound files
adb push res/snd/ $FTM_RESOUCE_PATH
