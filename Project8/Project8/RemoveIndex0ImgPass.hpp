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

using RemoveIndex0ImgPassBase = PassBase<StarImg&, bool>;
class RemoveIndex0ImgPass : public RemoveIndex0ImgPassBase
{
public:
    string GetName() override
    {
        return NAMEOF(RemoveIndex0ImgPass);
    }

    bool Calcurate(Mat& inout, StarImg& input) override
    {
        bool isRemoved;

        if (input.sourceIndex() == 0)
        {
            Point2i imgSize = inout.size();
            //inout = addSplitTextThickness(inout, "not convert", 50, 1, 1, convertRightUpCenter01ToCvScape(Point2f(-0.15, 0.25), imgSize));

            isRemoved = true;
        }
        else
        {
            isRemoved = false;
        }

        return isRemoved;
    }

    RemoveIndex0ImgPass() : RemoveIndex0ImgPassBase() { }
    ~RemoveIndex0ImgPass() { }
};
