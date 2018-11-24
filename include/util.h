/**
 * Utilities
 */
#ifndef UTIL_H
#define UTIL_H

void itoa(int n, char s[]);
void reverse(char s[]);
char* string_chop(char* base, char* sub);
char** array_append(char** a, char* s, int len);
char** str_split(char* s, char* delim);
char* append(char* s, char c);
void trim(char* s);


#endif

