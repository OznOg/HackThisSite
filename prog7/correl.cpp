#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

std::pair<int, double> findBestMatch(Mat img, Mat templ)
{
    int match_method = CV_TM_CCORR_NORMED;
    Mat result;
    result.create(img.rows, 1, CV_32FC1);

    /// Do the Matching and Normalize
    matchTemplate(img, templ, result, match_method );

//    normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

    /// Localizing the best match with minMaxLoc
    Point maxLoc;
    double maxVal;

    minMaxLoc(result, NULL, &maxVal, NULL, &maxLoc, Mat());

    return make_pair(maxLoc.y, maxVal);
}

Mat selectRowsContainingValue(const Mat &mat, int val)
{
    Mat img1 = Mat::zeros(mat.size(), CV_8UC1);
    mat.copyTo(img1, mat == val);

    Mat output;
    for (int rowIdx = 0; rowIdx < mat.rows; rowIdx++) {
        const auto &row = img1.row(rowIdx);
        if (countNonZero(row) > 0)
            output.push_back(row);
    }
    return output;
}

Mat unscramble(const Mat &img) {
    std::pair<int, double> best_match(0, 0);
    for (auto _row = 0; _row < img.rows; _row++) {
        int row = _row;
        Mat templ;
        Mat gni;
        img.copyTo(gni);
        double corrsum = 0;
        templ.create(1, gni.cols, gni.type());
        Mat unscrambled = Mat::zeros(gni.size(), gni.type());
        gni.row(row).copyTo(unscrambled.row(0));
        for (int count = 0; count < gni.rows; count++) {
            gni.row(row).copyTo(templ.row(0));
            gni.row(row).setTo(0);
            auto match = findBestMatch(gni, templ);
            gni.row(match.first).copyTo(unscrambled.row(count));
            row = match.first;
            corrsum += match.second;
        }

        if (best_match.second < corrsum)
            best_match = make_pair(_row, corrsum);

        imshow("Result window", unscrambled);
        imwrite("gni.png", unscrambled);
        waitKey(10);
    }
    cout << "best_match=" << best_match.first << std::endl;
    {
        int row = best_match.first;
        Mat templ;
        Mat gni;
        img.copyTo(gni);
        templ.create(1, gni.cols, gni.type());
        Mat unscrambled = Mat::zeros(gni.size(), gni.type());
        gni.row(row).copyTo(unscrambled.row(0));
        for (int count = 0; count < gni.rows; count++) {
            gni.row(row).copyTo(templ.row(0));
            gni.row(row).setTo(0);
            auto match = findBestMatch(gni, templ);
            gni.row(match.first).copyTo(unscrambled.row(count));
            row = match.first;
        }

        return unscrambled;
    }
}

/** @function main */
int main( int argc, char** argv )
{
    /// Load image and template
    Mat img = imread( argv[1], IMREAD_COLOR);

    Mat bgr;
    img.convertTo(bgr, CV_8UC3);

    // Split the image into different channels
    vector<Mat> bgrChannels(3);

    split(bgr, bgrChannels);

    imshow("Result b", bgrChannels[0]);
    imshow("Result g", bgrChannels[1]);
    imshow("Result r", bgrChannels[2]);

    auto backgroundcolor = bgrChannels[2].at<uint8_t>(0);
    Mat ref = Mat::zeros(bgrChannels[2].size(), CV_8U);

    /* remove background color */
    bgrChannels[2].copyTo(ref, (bgrChannels[2] > (backgroundcolor + 5)) + (bgrChannels[2] < (backgroundcolor - 5)));

    double maxVal = 0;
    minMaxLoc(ref, NULL, &maxVal, NULL, NULL, Mat());

    Mat img1 = selectRowsContainingValue(ref, maxVal) == 0;
    Mat ref2 = Mat::zeros(bgrChannels[2].size(), CV_8U);
    ref.copyTo(ref2, ref != maxVal);

    maxVal = 0;
    minMaxLoc(ref2, NULL, &maxVal, NULL, NULL, Mat());
    Mat img2 = (selectRowsContainingValue(ref2, maxVal) == 0);

    imshow("image 1", img1);
    imshow("image 2", img2);

    imshow("Result 1 window", unscramble(img1));
    imshow("Result 2 window", unscramble(img2));
    waitKey(0);
    return 0;
}

