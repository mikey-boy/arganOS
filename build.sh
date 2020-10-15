#!/bin/sh

if [ "$(pwd)" != "/home/mike/coding/osdev/bare-bones" ]; then
        echo "You are not in the proper directory"
        exit 1
fi

echo "Assembling bootstrap code"
nasm -felf32 boot.asm -o boot.o
echo "Compiling kernel"
i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
echo "Linking files"
i686-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o font/font.o -lgcc
echo "Creating ISO file\n"
grub-mkrescue -o myos.iso isodir
