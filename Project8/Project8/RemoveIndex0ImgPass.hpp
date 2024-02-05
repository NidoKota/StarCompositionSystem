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

    using RemoveIndex0ImgPassInput = tuple<StarImg, size_t>;
    using RemoveIndex0ImgPassBase = PassBase<RemoveIndex0ImgPassInput&&, bool>;
    class RemoveIndex0ImgPass : public RemoveIndex0ImgPassBase
    {
    public:
        string GetName() override
        {
            return NAMEOF(RemoveIndex0ImgPass);
        }

        bool Calcurate(Mat& inout, RemoveIndex0ImgPassInput&& input) override
        {
            auto& [starImg, starImgsSize] = input;

            bool isRemoved;

            if (starImg.sourceIndex() == 0 || starImg.sourceIndex() == (int)starImgsSize - 1)
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
}