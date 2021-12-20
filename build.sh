#!/usr/bin/env bash

PLATFORM=${1:-"digispark-tiny"}

echo $PLATFORM

echo "🧹 Cleaning"
rm -rf .pio/build # Clean out old builds
rm -rf .pio/libdeps # Clean out old library builds

echo "🧰 Compiling $PLATFORM"
pio run -e $PLATFORM -v

# For Digispark, we need to remove this troublesome file
rm .pio/libdeps/digispark-tiny/DigisparkMIDI/usbdrvasm.asm

# echo "💾 Uploading $PLATFORM"
pio run -e $PLATFORM -t nobuild -t upload -v

echo "👌🏾 Done"
