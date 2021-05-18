.PHONY: kernel dump qemu run qemu-dbg dbg gdb

kernel_elf := dreamos.elf
bin := dreamos.bin

default: qemu

kernel:
	scons

dump:
	scons --dump

build: kernel

clean:
	scons -c

qemu: build
	qemu-system-riscv64 -nographic -machine virt -m 256M -kernel $(bin)

qemu-dbg: build
	qemu-system-riscv64 -s -S -nographic -machine virt -m 256M -kernel $(bin)

run: qemu

dbg: qemu-dbg

gdb:
	gdbgui -p 10002 -r $(kernel_elf) -g "riscv64-unknown-elf-gdb --command=gdbcommand.txt"
