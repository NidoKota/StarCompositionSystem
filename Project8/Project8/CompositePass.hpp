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

namespace StarCompositionSystem
{
    using namespace std;
    using namespace cv;

    using CompositePassInput = tuple<StarImg, Mat>;
    using CompositePassBase = PassBase<CompositePassInput&&, void>;
    class CompositePass : public CompositePassBase
    {
    public:
        string GetName() override
        {
            return NAMEOF(CompositePass);
        }

        void Calcurate(Mat& inout, CompositePassInput&& input) override
        {
            auto& [starImg, resultImg] = input;

            Point2i resultImgSize = resultImg.size();

            float centerLat = starImg.lat();
            Mat scaledConvertedImg = Mat(resultImgSize.y / size(StarData::latIndexToLat), resultImgSize.x / StarData::latToLonImgCount.at(centerLat), CV_8UC3);
            Point2i scaledConvertedImgSize = scaledConvertedImg.size();
            Point2i scaledConvertedImgSizeAddOne = scaledConvertedImgSize + Point2i(1, 1);

            affine(scaledConvertedImg, inout, Point2i(0, 0), scaledConvertedImgSizeAddOne);

            int latIndex = starImg.latIndex();
            int lonIndex = starImg.lonIndex();

            Point2f leftUp = Point2i((StarData::latToLonImgCount.at(centerLat) - lonIndex - 1) * scaledConvertedImgSize.x, latIndex * scaledConvertedImgSize.y);
            Point2f rightDown = Point2i(((StarData::latToLonImgCount.at(centerLat) - lonIndex - 1) + 1) * scaledConvertedImgSizeAddOne.x, (latIndex + 1) * scaledConvertedImgSizeAddOne.y);

            affine(resultImg, scaledConvertedImg, leftUp, rightDown);
        }

        CompositePass() : CompositePassBase() { }
        ~CompositePass() { }
    };
}