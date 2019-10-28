#!/bin/sh

PROJ_DIR="/home/bc/shrd/lab/Project/2019_dsec_ctf"
INIT_DIR="/tmp/initrd_dbg"

## Unpack default cpio
#$PROJ_DIR/script/unpack_cpio.sh $PROJ_DIR/images/default/initramfs.cpio $INIT_DIR

## Update super_ez_kernel.ko
cd source
make
cp ./super_ez_kern.ko $INIT_DIR/super_ez_kern.ko
cd $PROJ_DIR

## Update exploit
cd $PROJ_DIR/exploit
./build.sh
cp ./exploit.elf64 $INIT_DIR/exploit
cd $PROJ_DIR

## Pack initrd
$PROJ_DIR/script/update_cpio.sh $INIT_DIR $PROJ_DIR/images/debug/initramfs.cpio

## spawn
cd images/debug
./run.sh