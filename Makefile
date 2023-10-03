# you must implement this makefile to create the following 8 targets for part 2 of the assignment:
# all, clean, clobber
# lc4
# lc4_disassembler.o, lc4_hash.o, lc4_loader.o, lc4_memory.o

all: lc4

lc4: lc4.o lc4_disassembler.o lc4_hash.o lc4_loader.o lc4_memory.o
	clang -g lc4.o lc4_disassembler.o lc4_hash.o lc4_loader.o lc4_memory.o -o lc4

lc4.o: lc4.c
	clang -g -c lc4.c -o lc4.o

lc4_disassembler.o: lc4_disassembler.c
	clang -g -c lc4_disassembler.c -o lc4_disassembler.o

lc4_hash.o: lc4_hash.c
	clang -g -c lc4_hash.c -o lc4_hash.o

lc4_loader.o: lc4_loader.c
	clang -g -c lc4_loader.c -o lc4_loader.o

lc4_memory.o: lc4_memory.c
	clang -g -c lc4_memory.c -o lc4_memory.o

clean:
	rm -f lc4.o lc4_disassembler.o lc4_hash.o lc4_loader.o lc4_memory.o

clobber: clean
	rm -f lc4