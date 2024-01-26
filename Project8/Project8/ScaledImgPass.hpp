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
#include "PassBase.hpp"

using namespace std;
using namespace cv;

using ScaledImgPassBase = PassBase<StarScale&, void>;
class ScaledImgPass : public ScaledImgPassBase
{
public:
    string GetName() override
    {
        return NAMEOF(ScaledImgPass);
    }

    void Calcurate(Mat& inout, StarScale& input) override
    {
        Point2i imgSize = inout.size();

        bool isUpLarger = input.upScaleImgSizeX > input.downScaleImgSizeX;

        Point2i bgSize(isUpLarger ? input.upScaleImgSizeX : input.downScaleImgSizeX, imgSize.y);
        Point2f bgCenterPix(bgSize.x / 2.0f, bgSize.y / 2.0f);
        Mat bg = Mat(bgSize.y, bgSize.x, CV_8UC3);

        /*Point2i plu = Point2i(0, 0);
        Point2i pru = Point2i(bgSize.x, 0);
        Point2i pld = Point2i(bgCenterPix.x - input.downScaleImgSizeX / 2.0f, bgSize.y);
        Point2i prd = Point2i(bgCenterPix.x + input.downScaleImgSizeX / 2.0f, bgSize.y);
        perspective(bg, inout, plu, pru, pld, prd);*/

        SplitScalerFunc scalerFunc = [input](int line) -> float
        {
            float lat = input.upLat - line * StarData::degPerPix;
            return 1.0f / cos((lat / 360.0f) * M_PI * 2.0f);
        };
        splitScale(bg, inout, SplitScalerDirection::X, scalerFunc);

        inout = bg;
    }

    ScaledImgPass() : ScaledImgPassBase() { }
    ~ScaledImgPass() { }
};
