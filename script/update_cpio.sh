#!/bin/bash

if [ $1 ] && [ $2 ] && [ -d $1 ]; then
    CPIO_PATH=$(realpath $1);
    OUT_PATH=$(realpath $2);
else
    echo "Usage: $0 [cpio folder path] [cpio result path]";
    exit
fi

if [ -d $2 ]; then
    echo "Usage: $0 [cpio folder path] [cpio result path]";
    exit
fi

ORIGDIR=`pwd`

cd $CPIO_PATH
find . | cpio -H newc -ov -F $OUT_PATH 2>/dev/null
cd $ORIGDIR

echo "Packed path $OUT_PATH"