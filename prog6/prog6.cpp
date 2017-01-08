/*
 * HTS programming mission 6 solver
 * (c) Sebastien Gonzalve 2017
 *
 * You need to save the image.html from HTS and run the script
 * ex:
 *    ./script
 *     
 * Need
 *  * opencv for image processing
 *  * gocr for character recognition
 *
 * compile on Linux with
 *    g++ `pkg-config --libs opencv` -Wall -Wextra prog6.cpp
 *
 * Nevertheless, compilation is performed when launching script
 *
 */
#include "prog6.h"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <iomanip>
#include <sys/stat.h> // for mkdir
#include <cerrno>

static const cv::Scalar white(255, 255, 255);

cv::Point operator/(cv::Point p, int)
{
    return cv::Point(p.x/2, p.y/2);
}

cv::Point2f operator-(cv::Point p, cv::Point2f f)
{
    return cv::Point2f(p.x -f.x, p.y -f.y);
}

static cv::Mat generateImage()
{
  // Create black empty image
  cv::Mat image = cv::Mat::zeros(1000, 1000, CV_8UC1);

  /* this is just a rewrite of the javascript inside the html page; I guess the
   * one who wrote it knows what he does */
  for (size_t i = 0; i < data.size(); /**/) {
      if (data[i + 2] >= 10) {
          int xend = data[i++], yend = data[i++],
              xstart = data[i++], ystart = data[i++];
          line(image, cv::Point(xend, yend), cv::Point(xstart, ystart), white);
      } else {
	  int xcenter = data[i++], ycenter = data[i++], radius = data[i++],
              arcstart = data[i++], arcextend = data[i++];
	  ellipse(image, cv::Point(xcenter, ycenter), cv::Size(radius, radius),
		  -arcstart, 0, -arcextend, white);
      }
  }
  return image;
}

static std::vector<std::vector<cv::Point>> findContours(cv::Mat image)
{
  cv::Mat threshold_output;
  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;

  int thresh = 73; // taken from openCV doc...
  // Detect edges using Threshold
  cv::threshold(image, threshold_output, thresh, 255, cv::THRESH_BINARY);
  // Find contours
  cv::findContours(threshold_output, contours, hierarchy, cv::RETR_EXTERNAL,
                   cv::CHAIN_APPROX_NONE, cv::Point(0, 0));

  return contours;
}
 
int main(int, char **)
{

  auto image = generateImage();
  cv::imwrite ("Image.jpg", image);

  auto contours = findContours(image);

  cv::Point2f center(0, 0); // pseudo center of the spiral
  std::vector<cv::Rect> boundingRects;

  /* Convert raw contours into rectangle bounding boxes */
  for (const auto &c : contours) {
      cv::Rect boundRect = cv::boundingRect(c);
      boundingRects.push_back(boundRect);

      center += cv::Point2f(boundRect.tl().x, boundRect.tl().y);
      center += cv::Point2f(boundRect.br().x, boundRect.br().y);
  }

  // Center is computed as the mean position of all the letters
  // This is not completely right as the image is symetric, but is correct
  // enougth for my computations
  center.x /= 2 * contours.size();
  center.y /= 2 * contours.size();

  // sort bondrect
  static constexpr size_t sector_count = 36; // counted from the image
  std::vector<std::vector<cv::Rect>> ordered(sector_count);

  const double Pi = 3.14159265;

  /* sort bounding boxes in function of their image sector */
  for (auto &boundingRect : boundingRects) {
      auto suma = (boundingRect.tl() + boundingRect.br()) / 2;
      auto ac = suma - center;
      auto at = atan2(ac.y, ac.x) + Pi / 2 /* origin is shifted of Pi/2 */
                + 2 * Pi /* Make sure value is > 0 */;

      int sector = ((int)((at * 180) / Pi + 5 /* 5 stands for half a sector */) % 360) / (360 / sector_count);
      ordered[sector].push_back(boundingRect);
  } 

  /* sort each sector in function of distance to center */
  for (auto &v : ordered)
      std::sort(v.begin(), v.end(), [&center](const cv::Rect &a, const cv::Rect &b) {
                  auto suma = (a.tl() + a.br()) / 2;
                  auto sumb = (b.tl() + b.br()) / 2;
                  auto ac = suma - center;
                  auto bc = sumb - center;
                  return norm(bc) > norm(ac);
                });

  /* Sort all bounding boxes starting from center and turning anti-clockwise
   * => round robin on angles to take the closer to center */
  std::vector<std::pair<cv::Rect, int>> sorted;
  int sector = 0;
  while (sorted.size() != boundingRects.size()) {
          auto &vect = ordered[sector];
          auto it = std::min_element(vect.begin(), vect.end(),
                  [&center](cv::Rect a, cv::Rect b) {
                     auto suma = (a.tl() + a.br()) / 2;
                     auto sumb = (b.tl() + b.br()) / 2;
                     auto ac = suma - center;
                     auto bc = sumb - center;
                     return norm(bc) > norm(ac);
                  });

      assert(it != vect.end());

      sorted.push_back(std::make_pair(*it, sector));
      vect.erase(it);
      sector++;
      sector %= 36;
  }

  // rotate all letters to get them in the right position before passing the ocr
  std::vector<cv::Mat> output;
  int i = 0;
  for (auto r : sorted) {
      cv::Mat dst;
      cv::Mat crop_img = cv::Mat(image, r.first);
      cv::Point2f rot_center(crop_img.cols/2, crop_img.rows/2);
      cv::Mat rot_mat = getRotationMatrix2D(rot_center, r.second * 10, 1);
      rot_mat.at<double>(0,2) += 15 - rot_center.x;
      rot_mat.at<double>(1,2) += 15 - rot_center.y;

      /// Rotate the warped image
      warpAffine(crop_img, dst, rot_mat, cv::Size(30, 30));
      output.push_back(dst);
  }

  // dump all images in out directory
  auto status = mkdir("./out/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (status != 0 && errno != EEXIST) {
      std::cerr << "Failed to create output directory 'out'." << std::endl;
      return 1;
  }

  i = 0;
  for (auto m : output) {
      std::stringstream ss;
      ss << "out/img" << std::setw(3) << std::setfill('0') << i++ << ".png";
      cv::imwrite(ss.str(), m);
  }


  // add numbering to image
  i = 0;
  for (auto r : sorted) {
      std::stringstream ss;
      ss << i++;
      cv::putText(image, ss.str(), r.first.tl(), 1, 1, white);
  }

  /// Show in a window
  // cv::imshow( "Contours", image );
  // cv::waitKey(0);
  return 0;
}
