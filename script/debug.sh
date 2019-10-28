#!/bin/sh

PROJ_DIR="/home/bc/shrd/lab/Project/2019_dsec_ctf"
INIT_DIR="/tmp/initrd_dbg"

## Unpack default cpio
$PROJ_DIR/script/unpack_cpio.sh $PROJ_DIR/images/default/initramfs.cpio $INIT_DIR

## Update super_modern_service
#cd super_modern_service
#./build.sh
#cp ./super_modern_service.elf64 $INIT_DIR/super_modern_service.elf64
#cd $PROJ_DIR

## Update exploit
#cd exploit
#./build.sh
#cp ./exploit $INIT_DIR/exploit
#cd $PROJ_DIR

## Pack initrd
$PROJ_DIR/script/update_cpio.sh $INIT_DIR $PROJ_DIR/images/debug/initramfs.cpio

## spawn
cd images/debug
./run.sh