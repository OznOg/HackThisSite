/*
 * HTS application 7
 * (c) Sebastien Gonzalve 2017
 *
 * This program is a reimplementation of code extracted form assembly
 *
 * compile on Linux with
 *    gcc -O3 -o app7 app7.cpp
 *
 */

#include <stdio.h>

int main(int argc, char **argv)
{
    FILE *filePtr;
    int storageIdx = 0;
    int encodedChar;
    int passSum = 0;
    int xorSum = 0;
    unsigned char storage[16] = { 0 };
    int tempChar;

    puts("Please enter the password:");

    while ((tempChar = getchar()) != '\n') {
        passSum += tempChar;
    }
    passSum += '\n';

    filePtr = fopen("encrypted.enc", "rb");

    if (filePtr == NULL) {
        puts("Failed to open encrypted.enc");
        return 12;
    }

    for (storageIdx = 0; storageIdx != 5; storageIdx++) {

        if (fread(&encodedChar, 1, 1, filePtr) != 1) {
            return 13;
        }

        encodedChar &= 0xFF;
        xorSum += encodedChar ^ passSum;

        storage[storageIdx] = (encodedChar ^ passSum) & 0xFF;

        for (int i = 0; i != passSum; i++) {
            if ((storage[storageIdx] & 1) != 0) {
                storage[storageIdx] >>= 1;
                storage[storageIdx] |= 0x80;
            } else {
                storage[storageIdx] >>= 1;
            }
        }
        storage[storageIdx] += 3;
    }

    fclose(filePtr);

    if (xorSum != 0xDCA) {
        puts("Invalid Password");
        return 14;
    }

    printf("Congratulations, The password is '%s'\n", storage);
    return 0;
}

