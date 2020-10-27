CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
ASMFLAGS = -f elf32
LINKFLAGS = -ffreestanding -O2 -nostdlib -lgcc

.PHONY: configure headers install clean

headers:
	$(MAKE) -C libc install-headers DESTDIR=$(SYSROOT) || exit $$?
	$(MAKE) -C kernel install-headers DESTDIR=$(SYSROOT) || exit $$?

install: headers
	$(MAKE) -C libc install DESTDIR=$(SYSROOT) || exit $$?
	$(MAKE) -C kernel install DESTDIR=$(SYSROOT) || exit $$?

clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C libc clean

	rm -rf sysroot
	rm -rf isodir
	rm -rf arganOS.iso
