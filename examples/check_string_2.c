//
// Created by Dylan Marinho on 23/04/27.
//

#include <stdio.h>

int main() {
  int i;
  int is_password = 1;

  char str1[11] = "mypassword";
  char str2[11] = "testpwd";

  for (i = 0; i < 10; i++) {
    if (str1[i] > str2[i]) {
      is_password = 0;
    } else if (str1[i] < str2[i]) {
      is_password = 0;
    }
  }

  if (is_password) {
    //printf(" Password is correct\n");
  } else {
    //printf(" Password is incorrect\n");
  }

  return 0;
}