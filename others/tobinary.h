#ifndef TOBINARY_H_
#define TOBINARY_H_

#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<math.h>

extern int chkint(char* str);
extern int chkfloat(char* str);
extern char * dec2bin(char *,char *);
extern char* flt2bin(char *str,char *pointer);

extern int logbase (int y, int b);
extern char* itoa(int num, char* str, int base);
extern void reverse(char str[], int length);

#endif
