// Source       : https://en.wikipedia.org/wiki/Timing_attack (secure version, as of 2024/01/19)
// Created by   : Étienne André and Didier Lime
// Created      : 2024/01/19
// Last modified: 2024/04/02

// #include <stdio.h>
#include <stddef.h>

// Needed to compile with m0plus
void *memset(char *b, int c, size_t len);

int main() {
    int i;
    int length = 10; // length of the char

    char ca[11] = "choudoufou";
    char cb[11] = "cheese";

    // NOTE: we use char for result to have the same memory size as the input passwords characters (otherwise, translation from 8 bits to 32 bits does not seem to be supported by our model)
    int result = 1; // true

    for (i = 0; i < length; i++){
         result &= (ca[i] == cb[i]);
    }

//     printf("Result: %d\n", result);

    return result;
}

void *memset(char *b, int c, size_t len) {
  while (len > 0) {
    *b++ = c;
    len--;
  }
}
