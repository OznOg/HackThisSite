/*
 * HTS programming mission 4 solver
 * (c) Sebastien Gonzalve 2017
 *
 * This program takes the 'plotMe.xml' file as only argument 
 * ex:
 *     ./prog4 plotMe.xml
 *
 * Need opencv for image manipulation primitives and tinyxml2 for xml parsing
 *
 * compile on Linux with
 *    g++ `pkg-config --libs opencv` `pkg-config --libs tinyxml2` -O3 -Wall -Wextra prog3.cpp
 *
 */
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "tinyxml2.h"
#include <iostream>
#include <stdio.h>
 
static const cv::Scalar &getColor(const std::string &txt)
{
    static cv::Scalar White(255, 255, 255);
    static cv::Scalar Red(0, 0, 255);
    static cv::Scalar Blue(255, 0, 0);
    static cv::Scalar Green(0, 255, 0);
    static cv::Scalar Yellow(0, 255, 255);

    if (txt == "red")
        return Red;
    else if (txt == "yellow")
        return Yellow;
    else if (txt == "green")
        return Green;
    else if (txt == "blue")
        return Blue;
    else
        return White;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " plotMe.xml" << std::endl;
        return 1;
    }

    // Create black empty images
    cv::Mat image = cv::Mat::zeros(1000, 1000, CV_8UC3);

    tinyxml2::XMLDocument xmlDoc;

    if (xmlDoc.LoadFile(argv[1]) != tinyxml2::XML_NO_ERROR) {
        std::cerr << "Error: cannot open '" << argv[1] << "'" << std::endl;
        return 2;
    }

    const tinyxml2::XMLNode *rootnode = xmlDoc.FirstChildElement("ppcPlot");

    for (const tinyxml2::XMLElement *element = rootnode->FirstChildElement("Line");
         element != NULL; element = element->NextSiblingElement("Line"))
    {
        int xend, yend, xstart, ystart;
        std::string color;
        element->FirstChildElement("XEnd")->QueryIntText(&xend);
        element->FirstChildElement("YEnd")->QueryIntText(&yend);
        element->FirstChildElement("XStart")->QueryIntText(&xstart);
        element->FirstChildElement("YStart")->QueryIntText(&ystart);
        color =  element->FirstChildElement("Color") ? element->FirstChildElement("Color")->GetText() : "";
        std::cout << xend << std::endl;
        std::cout << "<Line>\n <XEnd>" << xend << "</XEnd>\n <YEnd>" << yend <<
            "</YEnd>\n <XStart>" << xstart << "</XStart>\n <YStart>" << ystart
            << "</YStart>\n <Color>" << color<< "</Color>\n</Line>" << std::endl;
        cv::line(image, cv::Point(xend, 600 - yend), cv::Point(xstart, 600 - ystart), getColor(color));
    }
   
    for (const tinyxml2::XMLElement *element = rootnode->FirstChildElement("Arc");
         element != NULL; element = element->NextSiblingElement("Arc"))
    {
        int arcextend, arcstart, radius, xcenter, ycenter;
        std::string color;
        element->FirstChildElement("ArcStart")->QueryIntText(&arcstart);
        element->FirstChildElement("ArcExtend")->QueryIntText(&arcextend);
        element->FirstChildElement("XCenter")->QueryIntText(&xcenter);
        element->FirstChildElement("YCenter")->QueryIntText(&ycenter);
        element->FirstChildElement("Radius")->QueryIntText(&radius);
        color =  element->FirstChildElement("Color") ? element->FirstChildElement("Color")->GetText() : "";
        std::cout << "<Arc>\n <ArcStart>" << arcstart << "</ArcStart>\n <ArcExtend>" << arcextend <<
            "</ArcExtend>\n <XCenter>" << xcenter << "</XCenter>\n <YCenter>" << ycenter
            << "</YCenter>\n <Radius>" << radius<< "</Radius>\n <Color>" <<
            color<< "</Color>\n</Arc>" << std::endl;
        cv::ellipse(image, cv::Point(xcenter, 600 - ycenter), cv::Size(radius, radius), -arcstart, 0, -arcextend, getColor(color));
    }
   
  cv::imshow("Image",image);
  cv::imwrite("Image.jpg", image);
  cv::waitKey(0);

  return 0;
}
