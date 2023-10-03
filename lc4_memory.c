/************************************************************************/
/* File Name : lc4_memory.c		 										*/
/* Purpose   : This file implements the linked_list helper functions	*/
/* 			   to manage the LC4 memory									*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "lc4_memory.h"
#include <string.h>



int add_to_list (row_of_memory** head,
		 		 unsigned short int address,
		 		 unsigned short int contents)
{

	/* allocate memory for a single node */
	row_of_memory* new_row = malloc(sizeof(row_of_memory));
	if(new_row == NULL) {
		printf("error, could not allocate memory.\n");
		return -1;
	}
	/* populate fields in newly allocated node with arguments: address/contents */
	new_row->address = address;
	new_row->contents = contents;
    /* make certain to set other fields in structure to NULL */
	new_row->label = NULL;
	new_row->assembly = NULL;
	new_row->next = NULL;
	/* if head==NULL, node created is the new head of the list! */
	if (*head == NULL) {
		*head = new_row;
	}
	/* otherwise, insert the node into the linked list keeping it in order of ascending addresses */
	else {
		row_of_memory* curr_row = *head;
		if (curr_row->address > new_row->address) {
			new_row->next = curr_row;
			*head = new_row;
		}
		else {
			row_of_memory* next_row = curr_row->next;
			while (next_row != NULL) {
				//if the address is already in the list just updates the row
				if (curr_row->address == address){
					curr_row->contents = contents;
					free(new_row);
					return 0;
				}
				//else wait until the address is larger than the previous and insert the node there
				else if (next_row->address > new_row->address) {
					break;
				}
				else {
					curr_row = curr_row->next;
					next_row = curr_row->next;
				}
			}
			curr_row->next = new_row;
			new_row->next = next_row;
			return 0;
		}
	}

	/* return 0 for success, -1 if malloc fails */

	return 0 ;
}


/*
 * search linked list by address field, returns node if found
 */
row_of_memory* search_address (row_of_memory* head,
			        		   unsigned short int address )
{
	/* traverse linked list, searching each node for "address"  */
    row_of_memory* curr_row = head;
    while (curr_row != NULL) {
        /* return pointer to node in the list if item is found */
        if (curr_row->address == address) {
            return curr_row;
        } else {
            curr_row = curr_row->next;
        }
    }
	/* return NULL if list is empty or if "address" isn't found */
	return NULL ;
}

/*
 * search linked list by opcode field, returns node if found
 */
row_of_memory* search_opcode (row_of_memory* head,
				      		  unsigned short int opcode  )
{
	/* traverse linked list until node is found with matching opcode
	   AND "assembly" field of node is empty */
	   row_of_memory* curr_row = head;

	   while (curr_row != NULL) {
		   unsigned short int curr_opcode = (curr_row->contents & 0xF000) >> 12u;
		   if (curr_opcode == opcode && curr_row->assembly == NULL) {
			   /* return pointer to node in the list if item is found */
			   return curr_row; 
		   } else {
			   curr_row = curr_row->next;
		   }
	   }

	/* return NULL if list is empty or if no matching nodes */

	return NULL ;
}

/*
 * delete the node from the linked list with matching address
 */
int delete_from_list (row_of_memory** head,
			          unsigned short int address ) 
{
	/* if head isn't NULL, traverse linked list until node is found with matching address */
	if (*head == NULL) {
		printf("error: node not found\n");
		return -1;
	}
	else {
		row_of_memory* curr_row = *head;
		if (address == curr_row->address) {
			/* make certain to update the head pointer - if original was deleted */
			row_of_memory* temp = curr_row;
			*head = curr_row->next;
			//handles if memory was allocated for label and assembly sections
			if (temp->label != NULL) {
				free(temp->label);
			}
			if (temp->assembly != NULL) {
				free(temp->assembly);
			}
			//frees the node itself
			free(temp);
			return 0 ;
		}
		//handles if node is not head pointer
		else {
			row_of_memory* next_row = curr_row->next;
			while(next_row != NULL) {
				if (next_row->address == address) {
					/* delete the matching node, re-link the list */
					row_of_memory* temp = next_row->next;
					curr_row->next = temp;
					//handles if memory was allocated for label and assembly sections
					if (next_row->label != NULL) {
						free(next_row->label);
					}
					if (next_row->assembly != NULL) {
						free(temp->assembly);
					}
					//frees the node itself
					free(next_row);
					/* return 0 if successfully deleted the node from list, -1 if node wasn't found */
					return 0;
				}
				else {
					curr_row = curr_row->next;
					next_row = curr_row->next;
				}
			}
			/* return 0 if successfully deleted the node from list, -1 if node wasn't found */
			printf("error: node not found\n");
			return -1;
		}
	}

}

void print_list (row_of_memory* head, 
				 FILE* output_file )
{
	/* make sure head isn't NULL */
	if(head == NULL) {
		return;
	}
	/* print out a header to output_file */
	/* traverse linked list, print contents of each node to output_file */
	row_of_memory* curr_row = head;
	int i = 0;
	while(curr_row != NULL) {
		//handles the label printing
		if(curr_row->label != NULL) {
			if (strlen(curr_row->label) >= 11) {
				fprintf(output_file, "%s", curr_row->label);
			} else {
				int spaces = 11 - strlen(curr_row->label);
				fprintf(output_file, "%s", curr_row->label);
				fprintf(output_file,"%*s", spaces, "");
			}
		} else {
			fprintf(output_file, "           ");
		}
		//prints the address in hex
		fprintf(output_file, "%04X", curr_row->address);
		fprintf(output_file, "        ");
		//prints the contents in hex
		fprintf(output_file, "%04X", curr_row->contents);
		fprintf(output_file, "         ");
		//handles the assembly printing
		if (curr_row->assembly != NULL) {
			fprintf(output_file, "%s\n", curr_row->assembly);
		} else {
			fprintf(output_file, "\n");
		}
		curr_row = curr_row->next;
		i = i + 1;
	}
	return ;
}

/*
 * delete entire linked list
 */
void delete_list (row_of_memory** head )
{
	//do nothing if there is no list as the head
	if (head == NULL || *head == NULL) {
		return;
	}
    row_of_memory* curr_row = *head;
	//for each node frees label and assembly if necessary then frees node
    while(curr_row != NULL) {
        row_of_memory* temp = curr_row;
        curr_row = curr_row->next;
		temp->next = NULL;
		if (temp->label != NULL) {
			free(temp->label);
		}
		if (temp->assembly != NULL) {
			free(temp->assembly);
		}
		free(temp);
    }
	//deletes the reference to the list
	*head = NULL;
	return ;
}