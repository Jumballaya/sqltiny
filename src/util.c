/**
 * Utilities
 *
 * Mostly from K & R
 */
#include <string.h>

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
