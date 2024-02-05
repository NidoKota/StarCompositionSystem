#pragma once

#include <map>

namespace StarCompositionSystem
{
	using namespace std;

	class StarData
	{
	public:
		static const int latIndexToLat[];
		static const map<float, size_t> latToLonImgCount;
		static const map<float, float> latToLonStepMinit;
		static const map<float, float> latToLonStep;
		static const float latStep;
		static const float degPerPix;

		inline static string GetSoucesPath(string parentFolderPath, int index)
		{
			string result = parentFolderPath + "/H-" + to_string(index) + ".png";
			return result;
		}

		inline static int GetAllImgCount()
		{
			int result = 0;
			for (auto const& [lat, count] : StarData::latToLonImgCount)
			{
				result += count;
			}

			return result;
		}
	};

	// 緯度方向の要素数からその緯度(度数法)を返す
	inline const int StarData::latIndexToLat[] = 
	{
		90, 80, 70, 60, 50, 40, 30, 20, 10, 
		0,
		-10, -20, -30, -40, -50, -60, -70, -80, -90
	};

	// 緯度(度数法)からその緯度にある経度方向の画像達の数を返す
	inline const map<float, size_t> StarData::latToLonImgCount = 
	{ 
		{90, 1}, {80, 24}, {70, 36}, {60, 48}, {50, 60}, {40, 64}, {30, 72}, {20, 80}, {10, 80}, 
		{0, 80},
		{-10, 80}, {-20, 80}, {-30, 72}, {-40, 64}, {-50, 60}, {-60, 48}, {-70, 36}, {-80, 24}, {-90, 1}
	};

	// 緯度(度数法)からその緯度にある経度方向の画像達のそれぞれの角度(角度秒)の差を返す
	inline const map<float, float> StarData::latToLonStepMinit =
	{ 
		{90, 0}, {80, 60}, {70, 40}, {60, 30}, {50, 24}, {40, 22.5}, {30, 20}, {20, 18}, {10, 18},
		{0, 18},
		{-10, 18}, {-20, 18}, {-30, 20}, {-40, 22.5}, {-50, 24}, {-60, 30}, {-70, 40}, {-80, 60}, {-90, 0}
	};

	// 緯度(度数法)からその緯度にある経度方向の画像達のそれぞれの角度(度数法)の差を返す
	inline const map<float, float> StarData::latToLonStep =
	{
		{90, 360}, {80, 15}, {70, 10}, {60, 7.5}, {50, 6}, {40, 5.625}, {30, 5}, {20, 4.5}, {10, 4.5},
		{0, 4.5},
		{-10, 4.5}, {-20, 4.5}, {-30, 5}, {-40, 5.625}, {-50, 6}, {-60, 7.5}, {-70, 10}, {-80, 15}, {-90, 360}
	};

	// 緯度方向の画像達の角度(度数法)の差
	inline const float StarData::latStep = 10;

	// 緯度方向に1ピクセル移動するごとに緯度(度数法)が何度傾くか
	inline const float StarData::degPerPix = 0.02739726027;

	inline Mat AddDebugText(const StarImg& starImg, const string& addText = "", const float fontSize = 1, const int thickness = 1)
	{
		stringstream textStream;
		textStream << "lo=" << starImg.lon() << endl;
		textStream << "la=" << starImg.lat() << endl;
		textStream << "loi=" << starImg.lonIndex() << endl;
		textStream << "lai=" << starImg.latIndex() << endl;
		textStream << "soi=" << starImg.sourceIndex() << endl;
		textStream << addText << endl;

		//Mat img = useConvertedImg ? starImg.convertedImg() : starImg.img();
		Mat& img = starImg.img();
		Mat convertedImg = addSplitTextThickness(img, textStream.str(), 25, fontSize, thickness, convertRightUpCenter01ToCvScape(Point2f(-0.15, 0.25), Point2f(img.cols, img.rows)));

		return convertedImg;
	}

	inline Mat AddDebugText(const Mat& img, const StarImg& starImg, const string& addText = "", const float fontSize = 1, const int thickness = 1)
	{
		stringstream textStream;
		textStream << "lo=" << starImg.lon() << endl;
		textStream << "la=" << starImg.lat() << endl;
		textStream << "loi=" << starImg.lonIndex() << endl;
		textStream << "lai=" << starImg.latIndex() << endl;
		textStream << "soi=" << starImg.sourceIndex() << endl;
		textStream << addText << endl;

		Mat convertedImg = addSplitTextThickness(img, textStream.str(), 25, fontSize, thickness, convertRightUpCenter01ToCvScape(Point2f(-0.15, 0.25), Point2f(img.cols, img.rows)));

		return convertedImg;
	}
}