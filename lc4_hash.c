/************************************************************************/
/* File Name : lc4_hash.c		 										*/
/* Purpose   : This file contains the definitions for the hash table  	*/
/*																		*/
/* Author(s) : tjf 														*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "lc4_hash.h"

/*
 * creates a new hash table with num_of_buckets requested
 */
lc4_memory_segmented* create_hash_table (int num_of_buckets, 
             					         int (*hash_function)(void* table, void *key))
{
	// allocate a single hash table struct
    lc4_memory_segmented* new_hashtable = malloc(sizeof(lc4_memory_segmented));
    if(new_hashtable == NULL) {
        printf("error: could not create hashtable\n");
        return NULL;
    }

    // allocate memory for the buckets (head pointers)
    new_hashtable->buckets = malloc(sizeof(row_of_memory*) * num_of_buckets);
    if (new_hashtable->buckets == NULL) {
        printf("error: could not create buckets\n");
        free(new_hashtable);
        return NULL;
    }

    // initialize each bucket to NULL
    for (int i = 0; i < num_of_buckets; i++) {
        new_hashtable->buckets[i] = NULL;
    }

    // assign remaining fields
    new_hashtable->num_of_buckets = num_of_buckets;
    new_hashtable->hash_function = hash_function;

    return new_hashtable;
}


/*
 * adds a new entry to the table
 */
int add_entry_to_tbl (lc4_memory_segmented* table, 
					  unsigned short int address,
			    	  unsigned short int contents) 
{
	// apply hashing function to determine proper bucket #
	int bucket = (*table->hash_function)(table, &address); 
	// add to bucket's linked list using linked list add_to_list() helper function
	add_to_list(&table->buckets[bucket], address, contents);
	return 0 ;
}

/*
 * search for an address in the hash table
 */
row_of_memory* search_tbl_by_address 	(lc4_memory_segmented* table,
			                   			 unsigned short int address ) 
{
	// apply hashing function to determine bucket # item must be located in
	int bucket = (*table->hash_function)(table, &address);
	// invoked linked_lists helper function, search_by_address() to return return proper node
	return search_address(table->buckets[bucket], address);
}

/*
 * prints the linked list in a particular bucket
 */

void print_bucket (lc4_memory_segmented* table, 
				   int bucket_number,
				   FILE* output_file ) 
{
	// call the linked list helper function to print linked list
    print_list(table->buckets[bucket_number], output_file);
	return ;
}

/*
 * print the entire table (all buckets)
 */
void print_table (lc4_memory_segmented* table, 
				  FILE* output_file ) 
{
	// call the linked list helper function to print linked list to output file for each bucket
	fprintf( output_file, "<label>   <address>   <contents>   <assembly>\n");
    for(int i = 0; i < table->num_of_buckets; i++) {
        print_bucket(table, i, output_file);
    }
	return ;
}

/*
 * delete the entire table and underlying linked lists
 */

void delete_table (lc4_memory_segmented* table ) 
{
	if (table == NULL) {
		return;
	}
    // call linked list delete_list() on each bucket in hash table
	for(int i = 0; i < table->num_of_buckets; i++) {
		delete_list(&table->buckets[i]);
	}
	if (table->buckets != NULL) {
		free(table->buckets);
	}
	// then delete the table itself
	free(table);
	return ;
}
