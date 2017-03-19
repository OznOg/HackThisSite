/*
 * HTS programming mission 12 solver
 * (c) Sebastien Gonzalve 2017
 *
 * prog12 takes the 'string' as only parameter
 * compile on Linux with
 *    g++ -o prog12 prog12.cpp
 *
 */
#include <cctype>
#include <iostream>
#include <string>

static inline bool isPrime(uint8_t val) {
    switch (val) {
        case 2:
        case 3:
        case 5:
        case 7:
            return true;
        default:
            return false;
    }
}

/* In this challenge, you will be given a string. Take all the numbers from the
 * string and classify them as composite numbers or prime numbers. You should
 * assume all numbers are one digit, and neither number 1 nor number 0 counts.
 * Find the sum of every composite number, then find the sum of every prime
 * number. Multiply these sums together. Then, take the first 25 non-numeric
 * characters of the given string and increment their ASCII value by one (for
 * example, # becomes $). Take these 25 characters and concatenate the product
 * to them. This is your answer.*/

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "\n  Error: This program take the 'string' as the only parameter.\n\n";
        return 1;
    }

    const std::string code(argv[1]);

    if (code.length() != 640) {
        std::cerr << "\n  Error: The input string you gave =>{" << code
                  << "}<= does not have the required length (640 bytes).\n\n";
        return 2;
    }

    uint64_t prime_sum = 0, composite_sum = 0;
    std::string first25characters = "";

    for (const auto &c : code) {
        if (c == '1' || c == '0') // neither number 1 nor number 0 counts
            continue;

        if (std::isdigit(c)) {
            uint8_t val = c - '0';
            if (isPrime(val))
                prime_sum += val;
            else
                composite_sum += val;
        } else {
            if (first25characters.size() < 25) {
                first25characters.push_back(c + 1);
            }
        }
    }

    std::cout << first25characters << (uint64_t) prime_sum * composite_sum << std::endl;
    return 0;
}
