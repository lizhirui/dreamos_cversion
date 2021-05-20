.PHONY: all test

all: osdebugsupport osdebugsupport64

osdebugsupport: osdebugsupport.c
	gcc $< -o $@

osdebugsupport64: osdebugsupport.c
	gcc $< -o $@ -DARCH64

test: all
	./osdebugsupport64 ../../bsp/qemu-virt-rv64/dreamos.elf ../../bsp/qemu-virt-rv64/dreamos.bin