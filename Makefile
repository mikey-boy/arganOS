GCCPARAMS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
NASMPARAMS = -f elf32
LINKERPARAMS = -ffreestanding -O2 -nostdlib -lgcc

asmobjects = $(patsubst src/%.asm,objects/%.o,$(wildcard src/*.asm))
cobjects = $(patsubst src/%.c,objects/%.o,$(wildcard src/*.c))
objects = $(cobjects) $(asmobjects)

objects/%.o: src/%.asm
	   nasm $(NASMPARAMS) $< -o $@

objects/%.o: src/%.c
	   i686-elf-gcc $(GCCPARAMS) -o $@ -c $<

bin/kernel.bin: src/linker.ld $(objects)
	   i686-elf-gcc -T $< -o $@ $(objects) $(LINKERPARAMS)
