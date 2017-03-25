/*
 * HTS programming mission 10 solver
 * (c) Sebastien Gonzalve 2017
 *
 * This program takes the image as only argument.
 * Compilation requires
 *   - C++ 17 (for optional)
 *   - libcrypto++ for sha256 computation
 *   - libopencv for image manipulation
 *
 * compile on Linux with 
 *    g++ `pkg-config --libs opencv` `pkg-config --libs cryptopp` -O3  -std=c++1z -o prog10 prog10.cpp 
 *
 */

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/sha.h>
#include <cryptopp/base64.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <experimental/optional>

using std::experimental::optional;
using std::experimental::make_optional;

template <typename Hash>
class HashForcer {
    public:
        HashForcer(const std::string &aSum) : sum(aSum) {}

        auto force() {
            for (char c = 'z'; c >= 'a'; c--) {
                for (char C = 'Z'; C >= 'A'; C--) {
                    auto o = force(C, c, "");
                    if (o)
                        return o;
                }
            }
            return optional<std::string>();
        }

    private:
        optional<std::string> force(char U, char l, const std::string &s)
        {
            if (s.size() == length) {
                if (Hash().sum(s) == sum)  {
                    return make_optional(s);
                }
                return {};
            } else {
                auto o = force(U, l, std::string(s).append(1, l));
                if (o)
                    return o;
                else
                    return force(U, l, std::string(s).append(1, U));
            }
        }

        const std::string &sum;
        static constexpr size_t length = 10; // from HTS description
};

static std::string getBase64FromImage(const cv::Mat &_img)
{
    std::stringstream ss;

    cv::Mat img = _img;

    // Split the image into different channels
    std::vector<cv::Mat> rgbChannels(3);
    cv::split(img, rgbChannels);

    /* Find which color contains the data (must be non-zero and have no data in
     * the first column) */
    for (int i = 0; i < 3; i++)
    {
        cv::Mat &mat = rgbChannels[i];
        if (countNonZero(mat) == 0)
            continue;
        if (countNonZero(mat.col(0)) == 0) {
            // keep only the right color
            img = rgbChannels[i];
            break;
        }
    }

    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            if (img.at<char>(i, j) != 0) {
                ss << (char)j;
            }
        }
    }

    return ss.str();
}

static std::string getSha256FromBase64(const std::string &encoded)
{
    CryptoPP::Base64Decoder decoder;

    decoder.Put( (byte*)encoded.data(), encoded.size() );
    decoder.MessageEnd();

    size_t size = decoder.MaxRetrievable();
    if (size != 64 /* size of sha256 string */) {
        throw "Invalid base64 content";
    }

    std::string decoded;
    decoded.resize(size);
    decoder.Get((byte*)decoded.data(), decoded.size());

    return decoded;
}

struct Sha256
{
    std::string sum(const std::string &s)
    {
        std::vector<byte> out(CryptoPP::SHA256::DIGESTSIZE);
        CryptoPP::SHA256().CalculateDigest(&out[0], (byte *)s.c_str(), s.size());
        std::stringstream ss;

        for (auto b : out) {
            ss << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(b);
        }
        return ss.str();
    }
};

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cout << argv[0] << " takes image file as only argument." << std::endl;
        return 1;
    }

    cv::Mat src = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);

    if (!src.data) {
        std::cout <<  "Error: Could not open image file." << std::endl ;
        return 2;
    }

    auto base64 = getBase64FromImage(src);
    std::cout << "Extracted base64 from image: " << base64 << std::endl;

    auto sha256 = getSha256FromBase64(base64);
    std::cout << "Exctracted sha256: " << sha256 << std::endl;

    auto o = HashForcer<Sha256>(sha256).force();
    std::cout << "Matching password: " << o.value_or("No Match") << std::endl;

    return 0;
}
