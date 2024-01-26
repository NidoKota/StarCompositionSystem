#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

#include "CvUtility.hpp"

using namespace std;
using namespace cv;

class StarScale
{
public:
    float centerLat;
    float upLat;
    float downLat;
    float upScale;
    float downScale;
    float upScaleImgSizeX;
    float downScaleImgSizeX;
};