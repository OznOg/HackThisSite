/*
 * HTS application 7
 * (c) Sebastien Gonzalve 2017
 *
 * This program is a solver for application 7 challenge
 *
 * compile on Linux with
 *    g++ -O3 -o app7solver app7solver.cpp
 *
 */

#include <iostream>
#include <cstdio>


std::string generatePassword(int passSum)
{
    std::string pass;
    passSum -= '\n'; // remove the \n that is added to computation by the
                     // original program
    while (passSum != 0) {
        unsigned char letter = passSum > 'z' ? '0' : passSum;
        pass.append(1, letter);
        passSum -= letter;
    }
    return pass;
}

int main(int argc, char **argv)
{
    unsigned char encodedData[5];

    FILE *filePtr = fopen("encrypted.enc", "rb");

    if (filePtr == NULL) {
        std::cerr << "Error: Cannot open encrypted.enc file.\n";
        return 12;
    }

    if (fread(encodedData, 5, 1, filePtr) != 1) {
        std::cerr << "Error: Cannot read encrypted.enc file.\n";
        return 13;
    }

    fclose(filePtr);

    /* bruteforce the password's sum */
    for (int passSum = '\n'; // starts at \n because it is systematically added to sum
         passSum < 0x10000; passSum++) {
        int xorSum = 0xDCA;

        for (int storageIdx = 4; storageIdx >= 0; storageIdx--) {
            xorSum -= encodedData[storageIdx] ^ passSum;
        }
        if (xorSum == 0) {
            std::cout << "Correct sum is: " << passSum << std::endl;
            std::cout << "A valid password may be: " << generatePassword(passSum) << std::endl;
            return 0;
        }
    }

    return 1; // sum not found
}

