import os

asmf = open('dreamos.asm','r')
gdbtemplatef = open('gdbcommand_template.txt','r')
gdbf = open('gdbcommand.txt','w+')

line = asmf.readline()
found = False

while line:
    if line.find("jr	s2") >= 0:
        found = True
        break

    line = asmf.readline()

if not found:
    exit(-1)

arr = line.split(':')

address = '*' + hex(int(arr[0],16) - 0x2000000000 + 0x80000000)

line = gdbtemplatef.readline()

while line:
    line = line.replace("[before_virtual_address_space_entry]",address)
    gdbf.write(line)
    line = gdbtemplatef.readline()

gdbf.close()
gdbtemplatef.close()
asmf.close()