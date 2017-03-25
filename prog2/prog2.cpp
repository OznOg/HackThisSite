/*
 * HTS programming mission 2 solver
 * (c) Sebastien Gonzalve 2017
 *
 * This program takes the image as only argument.
 * Compilation requires
 *   - C++ 14
 *   - libopencv for image manipulation
 *
 * compile on Linux with
 *    g++ `pkg-config --libs opencv` -O3 -std=c++14 -o prog2 prog2.cpp
 *
 */
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <vector>

static const std::map < std::string, std::string > morse2Letter = {
    { ".-",    "a" }, { "-...",  "b" }, { "-.-.",  "c" }, { "-..",   "d" },
    { ".",     "e" }, { "..-.",  "f" }, { "--.",   "g" }, { "....",  "h" },
    { "..",    "i" }, { ".---",  "j" }, { "-.-",   "k" }, { ".-..",  "l" },
    { "--",    "m" }, { "-.",    "n" }, { "---",   "o" }, { ".--.",  "p" },
    { "--.-",  "q" }, { ".-.",   "r" }, { "...",   "s" }, { "-",     "t" },
    { "..-",   "u" }, { "...-",  "v" }, { ".--",   "w" }, { "-..-",  "x" },
    { "-.--",  "y" }, { "--..",  "z" }, { "-----", "0" }, { ".----", "1" },
    { "..---", "2" }, { "...--", "3" }, { "....-", "4" }, { ".....", "5" },
    { "-....", "6" }, { "--...", "7" }, { "---..", "8" }, { "----.", "9" },
    { "",       "" }
};


int main (int argc, char **argv)
{
    if (argc != 2) {
        std::cout << argv[0] << " takes image file as only argument." << std::endl;
        return 1;
    }

    cv::Mat img = cv::imread (argv[1], CV_LOAD_IMAGE_GRAYSCALE);

    if (!img.data) {
        std::cout << "Error: Could not open image file." << std::endl;
        return 2;
    }

    /* extract morse letters */
    std::vector<std::string> morse(1);
    unsigned last = 0;
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            if (img.at<char>(i, j) != 0) {
                auto curr = i * img.cols + j;
                auto letter = (char)(curr - last);
                if (letter == ' ')
                    morse.push_back("");
                else
                    morse.back().append(1, letter);
                last = curr;
            }
        }
    }

    for (auto m : morse)
        std::cout << morse2Letter.at(m);

    std::cout << std::endl;

    return 0;
}
