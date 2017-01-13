#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
 
using namespace std;
using namespace cv;
 
int main(int argc, char **argv)
{
int i;
int j = 0;
//for (i = 180; i < 199; i++)
do {
//j++; j = j %199;
{  
    Mat src = imread(argv[1], CV_LOAD_IMAGE_COLOR);    

    Mat frame_yuv;
    cvtColor( src, frame_yuv, CV_BGR2YUV );

    cv::Mat fi;
    frame_yuv.convertTo(fi, CV_32F);
    
    // Split the image into different channels
    vector<Mat> rgbChannels(3);

    split(fi, rgbChannels);
 
    fi = rgbChannels[1] + rgbChannels[2];// - (rgbChannels[2] - rgbChannels[2].at<float>(0)) ;
    //src -= 0x99;
    imshow("R", src);

    cv::Mat indices;

    cout << Mat (src, Range::all(), Range(46, 48));

    Mat floatIdx;
    cv::reduce(Mat(fi, Range::all(), Range(j, 199)), floatIdx, 1, CV_REDUCE_SUM);

    Mat new_idx;
    cv::sortIdx(floatIdx, new_idx, cv::SORT_EVERY_COLUMN | cv::SORT_ASCENDING);

    src = imread(argv[1], CV_LOAD_IMAGE_COLOR);    
    //rgbChannels[2].convertTo(src, src.type());
    Mat unscrambled = Mat::zeros(src.size(), src.type());
    for (int row = 0; row < src.rows; row++) {
         src.row(new_idx.at<int>(row)).copyTo(unscrambled.row(row));
    }

    
    cout << Mat (unscrambled, Range::all(), Range(46, 47));
    resize(unscrambled, unscrambled, Size(600, 300));
    imshow("E", unscrambled);
    if (waitKey(30) != -1)
        goto out;
}
} while (true);
out:

cout << i << " " << j << endl;
//waitKey(0);
exit(0);
#if 0

    cv::Mat fi;
    indices.convertTo(fi, CV_32F);

    Mat floatIdx;
    cv::reduce(fi, floatIdx, 1, CV_REDUCE_SUM);

    cout << "fi" << fi;
 
    Mat new_idx;
    cv::sortIdx(floatIdx, new_idx, cv::SORT_EVERY_COLUMN | cv::SORT_DESCENDING);

    cout << new_idx;
    for (int row = 0; row < src.rows; row++) {
        cout << (int)new_idx.at<int>(row) << endl;
    }

    //src = imread(argv[1], CV_LOAD_IMAGE_COLOR);    
    Mat unscrambled = Mat::zeros(src.size(), src.type());
    for (int row = 0; row < src.rows; row++) {
         src.row(new_idx.at<int>(row)).copyTo(unscrambled.row(row));
    }
    resize(unscrambled, unscrambled, Size(600, 300));
    imshow("E", unscrambled);
    waitKey(0);
#endif
    return 0;
}
