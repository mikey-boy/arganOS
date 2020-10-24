CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
ASMFLAGS = -f elf32
LINKFLAGS = -ffreestanding -O2 -nostdlib -lgcc

ARCHDIR := kernel/arch/i386

asmobjects = $(patsubst $(ARCHDIR)/%.asm,$(ARCHDIR)/%.o,$(wildcard $(ARCHDIR)/*.asm))
cobjects = $(patsubst kernel/%.c,kernel/%.o,$(wildcard kernel/*.c))
objects = $(cobjects) $(asmobjects)

.PHONY: all 

all: kernel.bin

kernel.bin: $(ARCHDIR)/linker.ld $(objects)
	i686-elf-gcc -T $< -o $@ $(objects) $(LINKFLAGS)
	grub-file --is-x86-multiboot2 $@
	
$(ARCHDIR)/%.o: $(ARCHDIR)/%.asm
	nasm $(ASMFLAGS) $< -o $@

kernel/%.o: kernel/%.c
	i686-elf-gcc $(CFLAGS) -o $@ -c $<

clean:
	rm kernel.bin
	rm $(objects)
