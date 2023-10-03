/************************************************************************/
/* File Name : lc4_disassembler.c 										*/
/* Purpose   : This file implements the reverse assembler 				*/
/*             for LC4 assembly.  It will be called by main()			*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include "lc4_hash.h"
#include <string.h>
#include <stdlib.h>

int parse_arithmatic_operation(row_of_memory* curr_row) {
    //definition for rs,rd,rt,imm5 that could become part of the final string assembly_string
    char assembly_string[100] = ""; 
    char rs[2];
	char rd[2];
	char rt[2];
	char imm5[6]; 
    //gets subop that is then analyzed by the switch statements
    unsigned short int subop = (curr_row->contents >> 3u) & 0x0007;
    //sets proper unsigned short int values into the defined strings above
    sprintf(rd, "%d", (curr_row->contents >> 9u) & 0x0007); 
    sprintf(rs, "%d", (curr_row->contents >> 6u) & 0x0007);
    sprintf(rt, "%d", curr_row->contents & 0x0007);
    sprintf(imm5, "%d", curr_row->contents & 0x001F);
    //builds the proper assembly statement depending on the subop
    switch(subop) {
        case 0:
            strcat(assembly_string, "ADD ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rd);
            strcat(assembly_string, " ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rs);
            strcat(assembly_string, " ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rt);
            break;
        case 1:
            strcat(assembly_string, "MUL ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rd);
            strcat(assembly_string, " ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rs);
            strcat(assembly_string, " ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rt);
            break;
        case 2:
            strcat(assembly_string, "SUB ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rd);
            strcat(assembly_string, " ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rs);
            strcat(assembly_string, " ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rt);
            break;
        case 3:
            strcat(assembly_string, "DIV ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rd);
            strcat(assembly_string, " ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rs);
            strcat(assembly_string, " ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rt);
            break;
        default:
            strcat(assembly_string, "ADD ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rd);
            strcat(assembly_string, " ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rs);
            strcat(assembly_string, " ");
            strcat(assembly_string, "#");
            strcat(assembly_string, imm5);
            break;
    }
    //allocates memory depending on the size of the final assembly_instruction
	curr_row->assembly = malloc(sizeof(char)*(strlen(assembly_string)+1));
    if (curr_row->assembly == NULL) {
        printf("error: could not allocate memory");
        return -1;
    }
    //copies final instruction into the hashtable
	strcpy(curr_row->assembly, assembly_string);
    
    return 0;
}
//parse_logical_operation is almost identical to parse_arithmatic_operation in its structure, but instead
//compiles the instruction for a logical operation
int parse_logical_operation(row_of_memory* curr_row) {
    char assembly_string[100] = "";
    char rs[2];
    char rd[2];
    char rt[2];
    char imm5[6];
    unsigned short int subop = (curr_row->contents >> 3u) & 0x0007;
    sprintf(rd, "%d", (curr_row->contents >> 9u) & 0x0007); 
    sprintf(rs, "%d", (curr_row->contents >> 6u) & 0x0007);
    sprintf(rt, "%d", curr_row->contents & 0x0007);
    sprintf(imm5, "%d", curr_row->contents & 0x001F);
    switch(subop) {
        case 0:
            strcat(assembly_string, "AND ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rd);
            strcat(assembly_string, " ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rs);
            strcat(assembly_string, " ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rt);
            break;
        case 1:
            strcat(assembly_string, "NOT ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rd);
            strcat(assembly_string, " ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rs);
            break;
        case 2:
            strcat(assembly_string, "OR ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rd);
            strcat(assembly_string, " ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rs);
            strcat(assembly_string, " ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rt);
            break;
        case 3:
            strcat(assembly_string, "XOR ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rd);
            strcat(assembly_string, " ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rs);
            strcat(assembly_string, " ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rt);
            break;
        default:
            strcat(assembly_string, "AND ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rd);
            strcat(assembly_string, " ");
            strcat(assembly_string, "R");
            strcat(assembly_string, rs);
            strcat(assembly_string, " ");
            strcat(assembly_string, "#");
            strcat(assembly_string, imm5);
            break;
    }
	curr_row->assembly = malloc(sizeof(char)*(strlen(assembly_string)+1));
    if (curr_row->assembly == NULL) {
        printf("error: could not allocate memory");
        return -1;
    }
	strcpy(curr_row->assembly, assembly_string);
	return 0;
}

int reverse_assemble (lc4_memory_segmented* memory) 
{
    //for each bucket search for rows with logical/arithmatic opcodes
	for(int i = 0; i < 4; i++) {
        if (memory->buckets[i] != NULL) {
            row_of_memory* curr_row = search_opcode(memory->buckets[i], 0x0001);
            //searches and parses until there are no arithmatic operations left to parse
            while(curr_row != NULL) {
                parse_arithmatic_operation(curr_row);
                curr_row = search_opcode(memory->buckets[i], 0x0001);
            }

            curr_row = search_opcode(memory->buckets[i], 0x0101);
            //searches and parses until there are no logical operations left to parse
            while(curr_row != NULL) {
                parse_arithmatic_operation(curr_row);
                curr_row = search_opcode(memory->buckets[i], 0x0101);
            }
        }
	}
	return 0 ;
}
