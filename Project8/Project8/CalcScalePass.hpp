#pragma once

#define _USE_MATH_DEFINES

#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <thread>
#include <vector>
#include <string>
#include <optional>
#include <tuple>

#include "StarImg.hpp"
#include "StarUtility.hpp"
#include "CvUtility.hpp"
#include "StarScale.hpp"
#include "PassBase.hpp"

using namespace std;
using namespace cv;

using CalcScalePassBase = PassBase<StarImg&, StarScale>;
class CalcScalePass : public CalcScalePassBase
{
public:
    string GetName() override
    {
        return NAMEOF(CalcScalePass);
    }

    StarScale Calcurate(Mat& inout, StarImg& input) override
    {
        StarScale output;

        Point2i imgSize = inout.size();
        Point2f imgCenterPix(imgSize.x / 2.0f, imgSize.y / 2.0f);

        output.centerLat = input.lat();
        output.upLat = output.centerLat + imgCenterPix.y * StarData::degPerPix;
        output.downLat = output.centerLat - imgCenterPix.y * StarData::degPerPix;

        output.upScale = 1.0f / cos((output.upLat / 360.0f) * M_PI * 2.0f);
        output.downScale = 1.0f / cos((output.downLat / 360.0f) * M_PI * 2.0f);

        output.upScaleImgSizeX = imgSize.x * output.upScale;
        output.downScaleImgSizeX = imgSize.x * output.downScale;

        return output;
    }

    CalcScalePass() : CalcScalePassBase() { }
    ~CalcScalePass() { }
};
