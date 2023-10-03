/************************************************************************/
/* File Name : lc4.c 													*/
/* Purpose   : This file contains the main() for this project			*/
/*             main() will call the loader and disassembler functions	*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "lc4_memory.h"
#include "lc4_hash.h"
#include "lc4_loader.h"
#include "lc4_disassembler.h"
#include "string.h"

/* program to mimic pennsim loader and disassemble object files */

int hash_function(void* table, void *key) {
	int addr = *(int*)key;
	//checks each range of memory and determines the bucket to return
	lc4_memory_segmented* memory = (lc4_memory_segmented*)table;
	if (addr >= 0x0000 && addr <= 0x1FFF ) {
		return 0;
	} else if (addr >= 0x2000 && addr <= 0x7FFF) {
		return 1;
	} else if (addr >= 0x8000 && addr <= 0x9FFF) {
		return 2;
	} else if (addr >= 0xA000 && addr <= 0xFFFF) {
		return 3;
	}
	return 0;
}

int main (int argc, char** argv) {
	//creates hashtable
	lc4_memory_segmented* memory = NULL ;
    memory = create_hash_table(4, hash_function);
	//checks if the correct number of arguments were provided
	if (argc < 3) {
		printf("error: you must specify the name of your output file and at least one object file\n");
		return -1;
	}
	//checks if the first file is a valid text file
	char *result = strstr(argv[1], ".txt");
	if (result == NULL) {
		printf("error: provided output file is not a text file");
		return -1;
	}
	//checks if each file provided is an object file and parses it into the hashtable if valid
	int file = 2;
	while(argv[file] != NULL) {
		result = strstr(argv[file], ".obj");
		if (result != NULL) {
			FILE* read_file = open_file(argv[file]);
			parse_file(read_file, memory);
			file = file + 1;
		}
		else {
			printf("error: provided file(s) are not object file(s)");
			return -1;
		}
	}
	//populates the assembly into the hashtable
	reverse_assemble(memory);
	//prints the now fully populated hashtable
	FILE* output_file = fopen(argv[1],"w");
	print_table(memory, output_file);
	fclose(output_file);
	//deletes the table and frees all memory
	delete_table(memory);
	return 0 ;
}

