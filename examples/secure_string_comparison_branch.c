// Source       : https://en.wikipedia.org/wiki/Timing_attack (secure version, as of 2024/01/19)
// Created by   : Étienne André and Didier Lime
// Created      : 2024/04/02
// Last modified: 2024/04/02

// #include <stdio.h>

int main() {
    int i;
    int length = 10; // length of the char

    char ca[11] = "choudoufou";
    char cb[11] = "cheese";

    // NOTE: we use char for result to have the same memory size as the input passwords characters (otherwise, translation from 8 bits to 32 bits does not seem to be supported by our model)
    int result = 1; // true

    for (i = 0; i < length; i++){
        if (result){
         result = (ca[i] == cb[i]);
        }else {
         result = 0;
        }
    }

//     printf("Result: %d\n", result);

    return result;
}
