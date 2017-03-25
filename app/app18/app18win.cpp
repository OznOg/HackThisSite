/*
 * HTS application mission 18 solver
 * (c) Sebastien Gonzalve 2017
 *
 * This program takes the login as parameter
 * ex:
 *     ./prog18 W4r10rdZ
 *
 * compile on Linux with
 *    g++ -O3 -Wall -Wextra -o app18win app18win.cpp
 *
 */

#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>


int32_t compute(int32_t edi, int rank, const std::string &str)
{
    constexpr int HTSsum = 'H' + 'T' + 'S';

    for (size_t pos = 0; pos < str.size(); pos++) {
        edi -= HTSsum;

        edi += 1;
        edi |= str[pos];
        uint32_t eax = edi;
        eax >>= rank;
        eax |= pos;

        edi ^= eax;
        edi *= edi;
    }
    return edi;
}

auto encode(const std::string &str)
{
    static constexpr auto iterations = 0x32;
    int32_t edi = 0;
    std::stringstream ss;
    ss << std::uppercase << std::hex;
    for (int rank = 0; rank < iterations; rank++) {
        edi = compute(edi, rank, str);
        if (rank == 0 || edi == 0)
            continue;
        ss << edi;
    }
    return ss.str();
}

int main(int argc, char **argv)
{
    if (argc > 2) {
        std::cout << "Invalid argument; This program takes the username as"
                     " only argument." << std::endl;
        return 1;
    }
    const std::string name = argc == 2 ? argv[1] : "ozlvog";

    if (name.size() < 4) {
        // Well, this is more or less buggy in the app18win.exe itself as HTS
        // has extra checks about username validity... (fe <= 32 bytes long)
        std::cout << "Invalid username; must be at least 4 characters long"
                  << std::endl;
        return 2;
    }

    std::cout << "generating license file for '" << name << "'" << std::endl;

    const std::string encoded = encode(name);
    std::ofstream licenseFile("app18win.lic", std::ios::binary | std::ios::trunc);

    if (!licenseFile) {
        std::cout << "Cannot open file\n";
        return 3;
    }

    licenseFile << "LIC" << '\0'
                << "1.8" << '\0'
                << std::setw(9) << std::setfill('0') << name.size() << '\0'
                << std::setw(9) << std::setfill('0') << encoded.size() << '\0'
                << "HTS" << '\0'
                << name << '\0'
                << encoded << '\0'
                << std::setw(10) << std::setfill('\0') << ""
                << "HTS_NQ" << '\0'
                << std::setw(10) << std::setfill('\0') << "";

    licenseFile.close();

    return 0;
}
