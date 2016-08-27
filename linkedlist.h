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

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include<stdlib.h>
#include<string.h>

#define MAXBF 50

struct list {
        char label[MAXBF];
        int memadd;
        struct list *next;
};

extern struct list *makenode(char *label);
extern void insert(struct list **head, struct list **tail, struct list *node);
extern struct list *search(struct list **head, char *label);
extern void cleanup(struct list **head, struct list **tail);
extern void printtable(struct list *head);

#endif
