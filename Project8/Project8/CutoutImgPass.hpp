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

using CutoutImgPassInput = tuple<StarImg&, StarScale&>;
using CutoutImgPassBase = PassBase<CutoutImgPassInput, void>;
class CutoutImgPass : public CutoutImgPassBase
{
public:
    string GetName() override
    {
        return NAMEOF(CutoutImgPass);
    }

    void Calcurate(Mat& inout, CutoutImgPassInput input) override
    {
        auto& [starImg, starScale] = input;

        Point2i imgSize = inout.size();

        float centerLat = starImg.lat();
        bool isUpLarger = starScale.upScaleImgSizeX > starScale.downScaleImgSizeX;

        Point2i bgSize(isUpLarger ? starScale.upScaleImgSizeX : starScale.downScaleImgSizeX, imgSize.y);
        Point2f bgCenterPix(bgSize.x / 2.0f, bgSize.y / 2.0f);

        int pixXRange = ceil(360.0f / StarData::degPerPix / StarData::latToLonImgCount.at(centerLat));
        int pixYRange = ceil(90.0f / StarData::degPerPix / (size(StarData::latIndexToLat) - 1));
        if (pixXRange % 2 != 0) pixXRange++;
        if (pixYRange % 2 != 0) pixYRange++;
        Point2i convertedImgSize = Point2i(pixXRange, pixYRange);

        Point2f clu = bgCenterPix - (Point2f)convertedImgSize / 2.0f;
        Point2f crd = bgCenterPix + (Point2f)convertedImgSize / 2.0f;
        inout = cutout(inout, clu, crd);
    }

    CutoutImgPass() : CutoutImgPassBase() { }
    ~CutoutImgPass() { }
};