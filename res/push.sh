#!/bin/bash

FTM_RESOUCE_PATH=/system/ftmres/

adb remount

echo ensure $FTM_RESOUCE_PATH exists
adb shell mkdir -p $FTM_RESOUCE_PATH

echo push bins
adb push bin/ $FTM_RESOUCE_PATH
adb shell "chmod +x $FTM_RESOUCE_PATH/bin/*"

echo push pictures
adb push img/ $FTM_RESOUCE_PATH

echo push sound files
adb push snd/ $FTM_RESOUCE_PATH
