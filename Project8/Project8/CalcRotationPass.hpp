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
#include "SideStarImg.hpp"

namespace StarCompositionSystem
{
    using namespace std;
    using namespace cv;

    using CalcRotationPassBase = PassBase<SideStarImg&, void>;
    class CalcRotationPass : public CalcRotationPassBase
    {
    public:
        string GetName() override
        {
            return NAMEOF(CalcRotationPass);
        }

        void Calcurate(Mat& inout, SideStarImg& input) override
        {
            StarScale output;

            if (input.left)
            {
                StarImg& left = input.left.value();
                //OUT("input.left");
            }

            if (input.right)
            {
                StarImg& right = input.right.value();
                //OUT("input.right");
            }

            /*Mat cimg = AddDebugText(input.center, false, "center");
            Mat limg = AddDebugText(left, false, "left");
            Mat rimg = AddDebugText(right, false, "right");

            showImage(cimg);
            showImage(limg);
            showImage(rimg);*/

            /*StarScale output;

            Mat cimg = AddDebugText(input.center, false, "center");

            showImageWaitAndThrough(cimg);

            OUT((input.left ? "L true" : "L false"));
            OUT((input.right ? "R true" : "R false"));*/
        }

        CalcRotationPass() : CalcRotationPassBase() { }
        ~CalcRotationPass() { }
    };
}