/**
 * Utilities
 *
 * Mostly from K & R
 */
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// String reverse from K & R 2nd Edition, page 62
void reverse(char s[]) {
  int c, i, j;

  for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

// Convert n characters to string s from K & R 2nd Edition, page 64
void itoa(int n, char s[]) {
  int i, sign;

  // Record sign and keep/make n positive
  if ((sign = n) < 0) n = -n;

  i = 0;

  // Generate digits in reverse order
  do {
    s[i++] = n % 10 + '0';
  } while ((n /= 10) > 0);

  if (sign < 0) s[i++] = '-';
  s[i] = '\0';
  reverse(s);
};

// Chops a substring from a string
char* string_chop(char* base, char* sub) {
  char* cpy;
  cpy = base;
  cpy += strlen(sub);
  return cpy;
};

// array append
char** array_append(char** a, char* s, int len) {
  char **p = malloc(sizeof(char*) * (len));
  memmove(p, a, sizeof(char*) * len);
  p[len] = s;
  return p;
}

// string split
char** str_split(char* s, char* delim) {
  // convert char* to char[]
  char str[strlen(s)];
  strcpy(str, s);

  int len = 0;
  char** ret = malloc(sizeof(char*) * strlen(s));
  char *token;

  // Go through the tokens and append to an array
  token = strtok(str, delim);
  while(token != NULL) {
    char* ns = malloc(sizeof(char*));
    strcpy(ns, token);
    ret = array_append(ret, ns, len);
    len += 1;
    token = strtok(NULL, delim);
  }

  return ret;
}

// string append
char* append(char* s, char c) {
  int len = strlen(s);

  char* p = malloc(sizeof(char) * (len + 1));
  strcpy(p, s);
  p[len] = c;
  p[len+1] = '\0';

  return p;
}

// string trim whitespace
void trim(char* s) {
  char *p = s;
  int len = strlen(p);

  while(isspace(p[len - 1])) {
    p[--len] = 0;
  }

  while(*p && isspace(*p)) {
    ++p;
    --len;
  }

  memmove(s, p, len + 1);
}



