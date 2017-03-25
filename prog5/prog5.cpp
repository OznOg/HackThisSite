/*
 * HTS programming mission 5 solver
 * (c) Sebastien Gonzalve 2017
 *
 * This program takes the 'corrupted.png.bz2' file as only argument
 * ex:
 *    ./prog5 corrupted.png.bz2
 *
 * compile on Linux with
 *    g++ -std=C++14 -O3 -Wall -Wextra -o prog5 prog5.cpp
 *
 */
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <experimental/optional>

using std::experimental::optional;
using std::experimental::make_optional;

static void dumpToFile(const std::string &file_name, const std::vector<char> &file_content) {
    std::ofstream os(file_name, std::fstream::binary);
    for (auto c : file_content)
        os << c;
    os.flush();
    os.close();
}

static bool isSoftwareInstalled(const std::string &binary_name)
{
    const std::string cmd("which " + binary_name + " >/dev/null 2>&1");
    return std::system(cmd.c_str()) == 0;
}

static int system(const std::string &cmd)
{
    return std::system(cmd.c_str());
}

class Prog5Solver
{
public:
    Prog5Solver(const std::vector<char> &a_file_content) :
        file_content(a_file_content), positions(findPositions(a_file_content)) {}

     optional<std::vector<char>> getValidContent()
     {
       return searchTree(positions.begin(), std::vector<char>(file_content.begin(),
                                                              file_content.begin() + positions[0]));
     }

private:
    optional<std::vector<char>> searchTree(const std::vector<int>::const_iterator &pos_iterator,
                                           const std::vector<char> &newFileHead)
    {
        if (pos_iterator == positions.end()) {
            const std::string file_name = "/tmp/test.bz2";
            try_count++;
            dumpToFile(file_name, newFileHead);
            if (fileIsValidBzip2(file_name)) {
                std::cout << "Found after " << try_count << " tries" << std::endl;
                return newFileHead;
            }
            if (try_count % 100 == 0)
                std::cout << try_count << " tested" << std::endl;
        } else {
            auto final_pos = pos_iterator + 1 == positions.end() ?
                std::end(file_content) : std::begin(file_content) + *(pos_iterator + 1);
            {
                auto v1 = newFileHead;
                v1.insert(std::end(v1), std::begin(file_content) + 1 + *pos_iterator, final_pos);
                auto match = searchTree(pos_iterator + 1, v1);
                if (match)
                    return match;
            }
            if (carriage_count < max_carriage_count) {
                auto v2 = newFileHead;
                v2.push_back('\r');
                v2.insert(std::end(v2), std::begin(file_content) + 1 + *pos_iterator, final_pos);
                carriage_count++;
                auto match = searchTree(pos_iterator + 1, v2);
                carriage_count--;
                if (match)
                    return match;
            }
        }
        return {};
    }

    static std::vector<int> findPositions(const std::vector<char> &file_content)
    {
        std::vector<int> positions;
        auto it = file_content.begin();
        while ((it = find(it, file_content.end(), '\r')) != file_content.end()) {
            /* Find all positions where '\r' is followed by '\n' */
            if (*(it + 1) == '\n')
                positions.push_back(std::distance(file_content.begin(), it));
            it++;
        }
        return positions;
    }
    static bool fileIsValidBzip2(const std::string &file_name)
    {
        const std::string cmd("bzip2 --test " + file_name + " >/dev/null 2>&1");
        return std::system(cmd.c_str()) == 0;
    }

    static const size_t max_carriage_count = 2; // seems to be enough... to be validated...
    size_t carriage_count = 0;
    size_t try_count = 0;

    const std::vector<char> &file_content;
    const std::vector<int> positions;
};

int main(int argc, char **argv)
{
    if (!isSoftwareInstalled("bzip2")
            || !isSoftwareInstalled("convert")
            || !isSoftwareInstalled("gocr")) {
        std::cerr << "Error: need gocr, bzip2 and convert to be installed." << std::endl;
        return 1;
    }

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " corrupted.png.bz2" << std::endl;
        return 1;
    }

    std::ifstream is(argv[1], std::fstream::binary);

    if (!is) {
        std::cerr << "Failed to open '" << argv[1] << "'" << std::endl;
        return 2;
    }

    std::istreambuf_iterator<char> start(is), end;

    std::vector<char> file_content(start, end);
    auto match = Prog5Solver(file_content).getValidContent();

    if (!match) {
        std::cout << "No suitable combination found." << std::endl;
        return 1;
    }

    const std::string validFileName = "valid.png.bz2";
    std::cout << "Writing " << validFileName << std::endl;
    dumpToFile(validFileName, match.value());

    std::cout << "Extracting..." << validFileName << std::endl;
    int ret = system("bzip2 --force --decompress " + validFileName);
    if (ret == 0) {
        std::cout << "Preparing 'valid.png' for gocr..." << std::endl;
        ret = system("convert -monochrome valid.png valid-mono.png");
    }

    std::cout << "Extracting with gocr:" << std::endl;
    if (ret == 0)
        ret = system("gocr valid-mono.png 2>/dev/null");
    else
        std::cout << "Failure." << std::endl;

    return ret;
}
