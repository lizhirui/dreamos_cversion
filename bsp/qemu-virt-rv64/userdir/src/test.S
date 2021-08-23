.global _start
_start:
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop

	li a7, 64
	li a0, 1
	lla a1, buf
	li a2, 13
	ecall
	j _start

buf: .string "Hello World!\n"
