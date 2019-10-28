#!/bin/sh

qemu-system-x86_64              \
    -no-reboot                  \
    -m 512M                     \
    -kernel bzImage             \
    -initrd initramfs.cpio      \
    -nographic                  \
    -append 'console=ttyS0 loglevel=3 oops=panic panic=-1 quiet kaslr' \
    -cpu kvm64,+smap,+smep      \
    -monitor /dev/null          \
    -s                          \
    2>/dev/null