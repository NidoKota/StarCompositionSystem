#pragma once

#include <iostream>
#include <optional>

#include <opencv2/opencv.hpp>

#include "StarImg.hpp"
#include "CvUtility.hpp"

namespace StarCompositionSystem
{
	using namespace std;
	using namespace cv;

	class SideStarImg
	{
	public:
		StarImg center;
		optional<StarImg> right = nullopt;
		optional<StarImg> left = nullopt;
	};
}