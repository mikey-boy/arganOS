#!/bin/sh

R='\033[0;31m'
G='\033[0;32m'
N='\033[0m'

check(){
    if [ $? -ne 0 ]; then 
        exit 1
    fi
}

echo -e "${G}[+]${N} Compiling the kernel"
make || check 

echo -e "${G}[+]${N} Replacing the new kernel"
rm isodir/boot/kernel.bin 2>/dev/null || cp bin/kernel.bin isodir/boot/kernel.bin || check

echo -e "${G}[+]${N} Creating ISO image"
grub-mkrescue -o arganOS.iso isodir || check
