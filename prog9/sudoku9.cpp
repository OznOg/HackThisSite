/*
 * HTS programming mission 9 sudoku solver
 * (c) Sebastien Gonzalve 2017
 *
 * This program takes the csv sudoku as only argument.
 * Compilation requires
 *   - C++ 17 (for optional)
 *
 * compile on Linux with 
 *    g++ -O3  -std=c++1z -o sudoku9 sudoku9.cpp 
 *
 */
#include <array>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <sstream>
#include <utility>
#include <vector>

#include <experimental/optional>

using std::experimental::optional;

using RawSudoku = std::array<uint8_t, 81>;

#define I
                           
static const RawSudoku easy = {
    // X --->
    0,5,6, I 9,0,7, I 4,0,0,
    0,8,1, I 0,4,0, I 0,0,0,  // Y
    0,0,0, I 0,1,5, I 0,9,0,  // |
 // ------ I ------ I ------     V
    0,0,0, I 0,0,3, I 8,5,7,
    8,4,0, I 0,6,0, I 0,2,3,
    7,3,9, I 2,0,0, I 0,0,0,
 // ------ I ------ I ------
    0,6,0, I 5,8,0, I 0,0,0,
    0,0,0, I 0,7,0, I 3,6,0,
    0,0,8, I 3,0,6, I 5,7,0 };

static const RawSudoku hard = {
    0,0,0, I 0,9,0, I 0,5,0,
    0,1,0, I 0,0,0, I 0,3,0,
    0,0,2, I 3,0,0, I 7,0,0,
 // ------ I ------ I ------
    0,0,4, I 5,0,0, I 0,7,0,
    8,0,0, I 0,0,0, I 2,0,0,
    0,0,0, I 0,0,6, I 4,0,0,
 // ------ I ------ I ------
    0,9,0, I 0,1,0, I 0,0,0,
    0,8,0, I 0,6,0, I 0,0,0,
    0,0,5, I 4,0,0, I 0,0,7 };

class Position {
public:
    constexpr Position(uint8_t x, uint8_t y) : _x(x), _y(y) {}

    uint8_t x() const { return _x; }
    uint8_t y() const { return _y; }

private:
    uint8_t _x, _y;
};

using Sudoku = std::array<std::vector<uint8_t>, 81>;

bool sectorHoldsValue(const Sudoku &s, const Position &p, uint8_t value) {
    for (uint8_t e = 0; e < 9; ++e) {
        const auto &pv = s[p.x() * 3 + (e % 3) + (p.y() * 3 + e / 3) * 9];
        if (pv.size() == 1 && pv.front() == value)
            return true;
    }
    return false;
}

bool colHoldsValue(const Sudoku &s, uint8_t idx, uint8_t value) {
    for (uint8_t y = 0; y < 9; ++y) {
        const auto &pv = s[idx + y * 9];
        if (pv.size() == 1 && pv.front() == value)
            return true;
    }
    return false;
}

bool lineHoldsValue(const Sudoku &s, uint8_t idx, uint8_t value) {
    for (uint8_t i = 0; i < 9; ++i) {
        const auto &pv = s[i + 9 * idx];
        if (pv.size() == 1 && pv.front() == value)
            return true;
    }
    return false;
}

Sudoku makeSudoku(const RawSudoku &sudoku) {
    Sudoku s;
    for (uint8_t e = 0; e < sudoku.size(); ++e) {
        if (sudoku[e] != 0) /* Value is already known */ {
            s[e].push_back(sudoku[e]);
            continue;
        }

        std::vector<uint8_t> v;
        for (uint8_t val = 1; val <= 9; ++val) {
            if (!sectorHoldsValue(s, Position((e % 9) / 3, (e / 9) / 3), val)
                    && !colHoldsValue(s, e % 9, val)
                    && !lineHoldsValue(s, e / 9, val))
                v.push_back(val);
        }
        if (v.empty())
            throw "Invalid input sudoku";

        s[e] = v;
    }
    return s;
}

std::string toString(const Sudoku &s) {
    std::stringstream ss;
    for (auto v : s) {
        ss << (int) v.front() << ",";
    }
    auto str = ss.str();
    str.resize(str.size() - 1); // remove trailing ','
    return str;
}

std::string toPrettyString(const Sudoku &s) {
    std::stringstream ss;
    for (uint8_t y = 0; y < 9; ++y) {
        for (uint8_t x = 0; x < 9; ++x) {
            const auto &pv = s[x + y * 9];
            ss << (int) (pv.size() == 1 ? pv.front() : 0) << " ";
            if (((x + 1) % 3) == 0 && x != 8)
                ss << "| ";
        }
        if (y != 8 && ((y + 1) % 3) == 0)
            ss << "\n------+-------+------";
        ss << std::endl;
    }
    return ss.str();
}

optional<Sudoku> solve(const Sudoku &sudoku) {
    for (uint8_t e = 0; e < sudoku.size(); ++e) {
        if (sudoku[e].size() == 1) /* Value is already known */ {
            continue;
        }

        for (const uint8_t &val : sudoku[e]) {
            std::div_t d = std::div(e, 9);
            if (!sectorHoldsValue(sudoku, Position(d.rem / 3, d.quot / 3), val)
                && !colHoldsValue(sudoku, d.rem, val)
                && !lineHoldsValue(sudoku, d.quot, val)) {
                Sudoku s = sudoku;
                s[e] = std::vector<uint8_t>(1, val);
                auto os = solve(s);
                if (os)
                    return os;
            }
        }

        /* Being here means that all values we have tested failed returning,
         * thus that we are in a dead branch */
        return {};
    }

    /* Here is the "normal" exit way: when only vectors of size 1 are present */
    return sudoku;
}

const RawSudoku getReference(int argc, char **argv) {
    if (argc == 1 || argc == 2 && std::string(argv[1]) == "--selftest") {
        return hard;
    }

    std::string input(argv[1]);

    auto v = std::accumulate(input.begin(), input.end(), std::vector<uint8_t>(),
                             [](auto &&v, auto && c) {
                                 if (c != ',')
                                     v.push_back(c - '0');
                                 return v;
                              });

    RawSudoku s;
    std::copy_n(v.begin(), s.size(), s.begin());
    return s;
}

int main(int argc, char **argv) {
    const RawSudoku &ref = getReference(argc, argv);

    Sudoku s = solve(makeSudoku(ref)).value();
    std::cout << toPrettyString(s) << "\n\n"
              << toString(s) << std::endl;

    return 0;
}

