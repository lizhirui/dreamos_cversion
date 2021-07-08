.PHONY: kernel root dump build clean qemu qemu-dbg run dbg gdb

kernel_elf := dreamos.elf
bin := dreamos.bin

default: qemu

kernel:
	scons

root:
	python ../../tools/mkromfs.py userdir/root src/romfs_data.c

dreamos.asm: dreamos.elf
	scons --dump

dump: dreamos.asm

gdbcommand.txt: gdbcommand_template.txt dreamos.asm
	python generate_gdbcommand.py

build: kernel

clean:
	scons -c

qemu: build dump
	qemu-system-riscv64 -nographic -machine virt -m 256M -kernel $(bin)

qemu-dbg: build dump gdbcommand.txt
	qemu-system-riscv64 -s -S -nographic -machine virt -m 256M -kernel $(bin)

run: qemu

dbg: qemu-dbg

gdb:
	gdbgui -p 10002 -r $(kernel_elf) -g "riscv64-unknown-elf-gdb --command=gdbcommand.txt"
