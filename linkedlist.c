/*
 * @author Parishkrit Jain <ptjain02@gmail.com>
 *
 * @section ABOUT
 *
 * This Program is a part of course project for COA course (2014-2015) offered 
 * to 4th semester students at LNMIIT, Jaipur. The project team members are
 * Parishkrit Jain, Nishant Gupta and Nikhil Saxena.
 *
 * @section DESCRIPTION
 * 
 * This is the linked list implementation for pnn assembler. This is supposed 
 * to be used for creating and using symbol table.
 */

#include<stdio.h>
#include "linkedlist.h"

struct list *makenode(char *label)
{
	struct list * symbol = (struct list *)malloc(sizeof(struct list));
	if(symbol != NULL) {
		strcpy((symbol)->label, label);
		(symbol)->memadd = 0;
		(symbol)->next = NULL;
	}
	return symbol;
}

void insert(struct list **head, struct list **tail, struct list *node)
{
	if(*head == NULL) {
		*head = node;
		*tail = node;
	}
	else {
		(*tail)->next = node;
		*tail = node;
	}
}

struct list *search(struct list **head, char *label)
{
	struct list * current = *head;
	while(current != NULL) {
		if(!strcmp((current->label), label))
			return current;
		current = current->next;
	}
	return NULL;
}

void cleanup(struct list **head, struct list **tail)
{
	struct list *node = *head;             
	struct list *temp;
	while(node != NULL) {     
	    temp = node;          
	    node = node->next;    
	    free(temp);      
	}
	*head = *tail = NULL; 
}

void printtable(struct list *head)
{
	struct list *curr;
	printf("\n\t| Memory Address | -> | Label Name |\n");
	printf("\t| -------------- | -- | ---------- |\n");
	for(curr = head; curr != NULL; curr = curr->next) {
		printf("\t|    0x%06X    | -> | %s |\n", 
					curr->memadd, curr->label);	
	}
}



