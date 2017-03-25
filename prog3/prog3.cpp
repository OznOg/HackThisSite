/*
 * HTS programming mission 3 solver
 * (c) Sebastien Gonzalve 2017
 *
 * This program takes the encrypted message entries as parameters
 * ex:
 *     ./prog3
 *
 * Need openssl for md5sum and boost for lexical cast
 *
 * compile on Linux with
 *    g++ `pkg-config --libs openssl` -fopenmp -O3 -Wall -Wextra prog3.cpp
 *
 */

#include <iostream>
#include <utility>
#include <vector>
#include <openssl/md5.h>
#include <boost/lexical_cast.hpp>

#include <cassert>

#include <experimental/optional>

namespace std {
    /* XXX keep this as long as optional is experimental */
    template <class T>
    using optional = experimental::optional<T>;
    #define make_optional experimental::make_optional
};

static int toInt(unsigned char value)
{
    if (value >= '0' && value <= '9')
        return value - '0';
    if (value >= 'a' && value <= 'f')
        return 10 + value - 'a';

    std::cout << "assertion failed " << value << " to be converted" << std::endl;
    assert(false);
}

static std::vector<unsigned char> __md5(const std::string &str)
{
    std::vector<unsigned char> digest(MD5_DIGEST_LENGTH);

    MD5((unsigned char*)str.c_str(), str.size(), digest.data());

    return digest;
}

static int evalCrossTotal(const std::vector<unsigned char> &bytesMD5)
{
    int sum = 0;
    for (const auto &val : bytesMD5) {
        sum += (val & 0xF) + ((val >> 4) & 0xF);
    }
    return sum;
}

static int computeIntMD5Total(const std::string &message, int prevIntMD5Total)
{
    if (message.empty())
        return prevIntMD5Total;

    auto messageMd5 = __md5(message);
    auto sumMd5 = __md5(boost::lexical_cast<std::string>(prevIntMD5Total));
    messageMd5.resize(8);
    sumMd5.resize(8);

    return evalCrossTotal(messageMd5) + evalCrossTotal(sumMd5);
}

static std::vector<int> __encryptString(const std::string &strString,
                                        const std::string &strPasswordMD5,
                                        int intMD5Total)
{
    std::vector<int> encryptedValues;

    for (size_t i = 0; i < strString.size(); i++) {
        encryptedValues.push_back((int)strString[i] + toInt(strPasswordMD5[i % 32]) - intMD5Total);
        intMD5Total = computeIntMD5Total(strString.substr(0, i + 1), intMD5Total);
    }
    return encryptedValues;
}

// $strString is the content of the entire file with serials
static std::string encryptString(const std::string &strString,
                                 const std::string &strPasswordMD5, int intMD5Total)
{
    std::stringstream ss;
    for (const auto &val : __encryptString(strString, strPasswordMD5, intMD5Total)) {
        ss << boost::lexical_cast<std::string>(val) << " ";
    }
    return ss.str();
}//-----------------------------------------------------------------------------------

class Prog3Solver {
public:
    Prog3Solver(const std::vector<int> &anEncrypted) : encrypted(anEncrypted) {}

    std::optional<std::string> force() const
    {
        std::optional<std::string> res;
        static bool cancel = false;

        // fist element is
        // [hex value of first serial letter] + [first letter of md5 as int] - Md5sum
        // thus Md5sum = [48 .. 90] + [0 .. 15] - encrypted[0]
        // => Md5sum is in range [48 .. 105] - encrypted[0]
        // as I considere that it is quite unlikely that [48 .. 90] + [0 .. 15] == 48
        // is begin at midway and search around mean first
        #pragma omp parallel shared(cancel)
        #pragma omp for schedule(auto)
        for (auto i = -29; i < 29; i++) {
            auto sum = -encrypted[0] + i + 48 + 29;
            auto hash = findHash(cancel, std::make_pair(std::string(""), sum), "");
            if (hash) {
                // the cancel boolean is here to 'forward' cancellation to
                // function findHash so that exploration of the tree can be
                // aborted if one thread already found the correct hash
                cancel = true;
                #pragma omp cancel for
                res = hash;
            }
            // Check for cancellations signalled by other threads:
            #pragma omp cancellation point for
        }
        return res;
    }

    std::optional<std::string> decode() const
    {
        /* no hash given: attempt to force the algo */
        auto hash = force();
        if (!hash) {
            std::cerr << "Cannot find hash for input." << std::endl;
            return {};
        }

        std::cout << "Found hash: " << hash.value() << std::endl;
        return decode(hash.value());
    }

    std::optional<std::string> decode(const std::string &hash) const
    {
        auto head = std::make_pair(std::string(""), sum(hash));
        while (head.first.size() < encrypted.size()) {
            auto newHead = decodeNext(head, hash);
            if (!newHead)
                return {};
            head = newHead.value();
        }
        return head.first;
    }

private:
    bool charIsValid(int pos, unsigned char new_char) const
    {
        /* Some letters are predefined depending on their position as they
         * follow the pattern: XXX-XXX-OEM-XXX-1.1\n */
        switch (pos % 20) {
            case 3:
            case 7:
            case 11:
            case 15:
                return new_char == '-';
            case 8:
                return new_char == 'O';
            case 9:
                return new_char == 'E';
            case 10:
                return new_char == 'M';
            case 16:
            case 18:
                return new_char == '1';
            case 17:
                return new_char == '.';
            case 19:
                return new_char == '\n';

            default:
                /* position does not constrain the letter: just check it is
                 * suitable ascii */
                return (new_char >= '0' && new_char <= '9')
                       || (new_char >= 'A' && new_char <= 'Z');
        }
    }

    std::optional<std::pair<std::string, int>> decodeNext(const std::pair<std::string, int> &head,
                                                          const std::string &pass) const
    {
        int new_pos = head.first.size();
        int new_total = computeIntMD5Total(head.first, head.second);

        unsigned char new_char = new_total + encrypted[new_pos] - toInt(pass[new_pos % 32]);

        if (!charIsValid(new_pos, new_char))
            return {};

        auto newSerial = std::string(head.first).append(1, new_char);

        return make_pair(newSerial, new_total);
    }

    inline std::optional<std::string> findHash(bool &cancel, const std::pair<std::string, int> &head,
                                        const std::string &hash) const
    {
        if (cancel)
            return {};
        if (hash.size() < 32) {
            /* hash < 32 means that it was not completely "guessed"; this means
             * that we must keep recursively explorate the possible values */
            for (auto letter = '0'; letter <= 'f';
                 letter == '9' ? letter = 'a' : letter++) {
                auto newHash = hash;
                newHash.append(1, letter); // add letter to previous hash

                auto newHead = decodeNext(head, newHash); // Challenge new letter
                if (!newHead)
                    continue; /* letter does not match, try next one... */

                /* letter _may_ be correct, go on with next one */
                auto goodHash = findHash(cancel, newHead.value(), newHash);
                if (goodHash)
                    return goodHash;
            }
        } else {
            /* being here means that 32 letters of the hash were found;
             * nevertheless, this does not afford that the hash is the correct
             * one, thus we need to decode the whole message to make sure the
             * hash is the correct one. */
            /* FIXME actually, testing 34 entries seems enought... but I would
             * need to get convinced that it is always the case before changing
             * the test here... */
            if (head.first.size() >= encrypted.size())
                return hash; // whole message is decyphered => hash is correct

            auto newHead = decodeNext(head, hash);
            if (!newHead)
                return {}; // hash is wrong for the rest of text => fail

            return findHash(cancel, newHead.value(), hash); // try next message's entry
        }

        return {};
    }

    int sum(const std::string &hash) const
    {
        int s = 0;
        for (const auto &c : hash)
            s += toInt(c);
        return s;
    }

    const std::vector<int> &encrypted;
};

bool selftest() {
    std::cout << "Perfoming self test...";
    const std::vector<int> test = { -157, -198, -213, -157, -154, -145, -181, -164, -153, -155, -130,
                                -150, -171, -149, -182, -218, -179, -178, -192, -253, -181, -124,
                                -157, -189, -163, -201, -158, -231, -138, -167, -181, -157, -181,
                                -156, -163, -188, -212, -167, -204, -244 , -85, -142, -153, -172,
                                -146, -206, -188, -156, -150, -141, -135, -193, -121, -170, -173,
                                -149, -218, -194, -163, -225, -165, -148, -119, -203, -157, -233,
                                -143, -189, -130, -180, -156, -167, -157, -169, -159, -162, -206,
                                -205, -223, -227, -176, -119, -166, -215, -147, -138, -170, -201,
                                -166, -186, -175, -180, -133, -141, -190, -163, -170, -160, -188, -222};
    const std::string result("L20-7ZQ-OEM-18F-1.1\nDQ5-QEB-OEM-FO9-1.1\nU7B-G2Q-OEM-N40-1.1\n"
            "MLC-F7F-OEM-GDF-1.1\n8FO-NUF-OEM-SLV-1.1\n");

    Prog3Solver solver(test);
    auto serials = solver.decode();
    if (serials && serials.value() == result) {
        std::cout << "Success" << std::endl;
        return true;
    } else {
        std::cout << "failed" << std::endl;
        return false;
    }
}

int main(int argc, char **argv)
{
    if (argc == 5 && std::string(argv[1]) == "check") {
        const std::string passMd5(argv[3]);
        std::vector<unsigned char> v(passMd5.begin(), passMd5.end());
        std::cout << evalCrossTotal(v) << std::endl;
        std::cout << encryptString(argv[2], passMd5,  boost::lexical_cast<int>(argv[4]))
                  << std::endl << std::endl;
        return 0;
    }

    if (argc == 2 && std::string(argv[1]) == "selftest") {
        return selftest() ? 0 : 1;
    }

    if (argc < 100) {
        std::cerr << argv[0] << " takes 100 arguments." << std::endl
                  << argv[0] << " -157 -198 -213 -157 -154 -145 -184 ..." << std::endl;
        return 1;
    }

    std::vector<std::string> args(argv + 1, argv + argc);

    std::vector<int> encrypted;
    for (const auto &arg : args) {
        encrypted.push_back(boost::lexical_cast<int>(arg));
    }

    Prog3Solver solver(encrypted);
    auto serials = solver.decode();
    if (serials)
        std::cout << "\nSerial numbers are:\n" << serials.value() << std::endl;
    else
        std::cerr << "Cannot decode input" << std::endl;
    return 0;
}

