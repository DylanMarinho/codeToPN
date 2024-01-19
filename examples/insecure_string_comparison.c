// Source: https://en.wikipedia.org/wiki/Timing_attack (insecure version, as of 2024/01/19)
// Created by Étienne André and Didier Lime
// Created: 2024/01/19
// Last modified: 2024/01/19

// #include <stdio.h>

int main() {
    int i;
    int length = 10; // length of the char

    char ca[11] = "choudoufou";
    char cb[11] = "cheese";

    for (i = 0; i < length; i++){
        if (ca[i] != cb[i]) {
//             printf("Different\n");
            return 0; // false
        }
    }
//     printf("Same\n");
    return 1; // true
}
