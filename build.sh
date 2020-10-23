#!/bin/sh

R='\033[0;31m'
G='\033[0;32m'
N='\033[0m'

echo -e "${G}[+]${N} Set up project directories if they don't exist"
[ -d bin ] || mkdir bin
[ -d objects ] || mkdir objects
[ -d isodir/boot ] || mkdir -p isodir/boot

echo -e "${G}[+]${N} Compiling the kernel"
make || exit 1 

echo -e "${G}[+]${N} Replacing the new kernel"
rm isodir/boot/kernel.bin 2>/dev/null
cp bin/kernel.bin isodir/boot/kernel.bin || exit 1

echo -e "${G}[+]${N} Creating ISO image"
grub-mkrescue -o arganOS.iso isodir || exit 1
