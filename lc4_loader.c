/************************************************************************/
/* File Name : lc4_loader.c		 										*/
/* Purpose   : This file implements the loader (ld) from PennSim		*/
/*             It will be called by main()								*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "lc4_memory.h"
#include "lc4_hash.h"

/* declarations of functions that must defined in lc4_loader.c */

//function that swaps the endian of the instructions passed in
unsigned short int swap_endian (unsigned short int instruction) {
  unsigned short int temp = instruction;
  unsigned short int s0,s1,s2,s3;
  s0 = (temp & 0x000F) << 8u;
  s1 = (temp & 0x00F0) << 8u;
  s2 = (temp & 0x0F00) >> 8u;
  s3 = (temp & 0xF000) >> 8u;
  return s0 + s1 + s2 + s3;
}



int parse_header(lc4_memory_segmented* memory, FILE* my_obj_file) {
    //initiallizes and reads in necessary variables
    unsigned short int addr, n, curr_instruction;
    fread(&addr, sizeof(addr), 1, my_obj_file);
    addr = swap_endian(addr);
    fread(&n, sizeof(n), 1, my_obj_file);
    n = swap_endian(n);
    //starting from the address, add entries to the table for the number of instructions n
    //that the object file states are a part of the code or data section
    for (int i = 0; i < n; i++) {
        fread(&curr_instruction, sizeof(curr_instruction), 1, my_obj_file);
        curr_instruction = swap_endian(curr_instruction);
        add_entry_to_tbl(memory, addr, curr_instruction);
        addr = addr + 1;
    }
    return 0;
}

int parse_symbol(lc4_memory_segmented* memory, FILE* my_obj_file) {
    //initiallizes and reads in necessary variables
    unsigned short int addr, n;
    char curr_char, next_char;
    fread(&addr, sizeof(addr), 1, my_obj_file);
    addr = swap_endian(addr);
    fread(&n, sizeof(n), 1, my_obj_file);
    n = swap_endian(n);
    //searches for row_of_memory at the address in the file
    row_of_memory* label_change = search_tbl_by_address(memory, addr);
    //creates empty table if the label's address is not in the table
    if (label_change == NULL) {
        add_entry_to_tbl(memory, addr, 0x0000);
        label_change = search_tbl_by_address(memory, addr);
    }
    //allocates an array in the heap to store the label
    label_change->label = malloc(sizeof(char)*(n+1));
    //puts the instructions in the file up to the length n into the file
    //if statements handle endian edge case when the label is an odd number of chars
    if (n % 2 == 0) {
        for (int i = 0; i < n; i += 2) {
            fread(&curr_char, sizeof(curr_char), 1, my_obj_file);
            fread(&next_char, sizeof(next_char), 1, my_obj_file);
            label_change->label[i] = curr_char;
            label_change->label[i+1] = next_char;
        }
    } else {
        for (int i = 0; i < n-1; i += 2) {
            fread(&curr_char, sizeof(curr_char), 1, my_obj_file);
            fread(&next_char, sizeof(next_char), 1, my_obj_file);
            label_change->label[i] = curr_char;
            label_change->label[i+1] = next_char;
        }
        fread(&curr_char, sizeof(curr_char), 1, my_obj_file);
        label_change->label[n-1] = curr_char;
    }
    label_change->label[n] = '\0'; // add null terminator to the end of the label
    return 0;
}

//opens the file to be parsed in read binary mode
FILE* open_file(char* file_name)
{
	FILE* fb = fopen(file_name, "rb");
    if (fb == NULL) {
        printf("error: could not open file\n");
        return NULL;
    } else {
        return fb;
    }
}

int parse_file (FILE* my_obj_file, lc4_memory_segmented* memory)
{
    unsigned short int binary_instruction;
    //reads to the end of the file and parses instructions as headers show up in the file
    while(!feof(my_obj_file)) {
        fread(&binary_instruction, sizeof(binary_instruction), 1, my_obj_file);
        binary_instruction = swap_endian(binary_instruction);
        if(binary_instruction == 0xCADE) {  //handles xCADE header
            parse_header(memory, my_obj_file);
        } else if (binary_instruction == 0xDADA) {  //handles xDADA header
            parse_header(memory, my_obj_file);
        } else if (binary_instruction == 0xC3B7) {  //handles xC3B7 (symbol) header
            parse_symbol(memory, my_obj_file);
        }
    }
    //close file
    int result = fclose(my_obj_file);
    if (result == 0) {
        return 0; 
    } else {
        printf("error: could not close file");
        return 1;
    }
}
