#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <tuple>

#include <opencv2/opencv.hpp>

#include "StarImg.hpp"
#include "StarImgLoader.hpp"
#include "StarImgConverter.hpp"
#include "CvUtility.hpp"

#include "Console.hpp"

using namespace std;
using namespace cv;

StarImgLoader starImgLoader = StarImgLoader();
StarImgConverter starImgConverter = StarImgConverter();
vector<StarImg> starImgs;
Mat result;

function<void(tuple<string, bool>)> onLoad = [&](tuple<string, bool> args)
{
    auto& [loadpath, multiThread] = args;
    starImgLoader.Load(starImgs, loadpath, multiThread);
};

function<void(bool)> onConvert = [&](bool multiThread)
{
    starImgConverter.Convert(result, starImgs, multiThread);
    saveImage(result);
};

function<void(tuple<int, string>)> onPreview = [&](tuple<int, string> args)
{
    auto& [sourceIndex, endPassName] = args;
    Mat m = starImgConverter.SingleConvert(starImgs, sourceIndex, endPassName);
    showImageWaitAndThrough(m);
};

function<void(void)> onKill = [&]()
{
    starImgLoader.Kill();
    starImgConverter.Kill();
};

int main(int argc, char* argv[])
{
    Console console(argv[0]);

    starImgLoader.OnError.Subscribe(console.onError);
    starImgLoader.OnComplete.Subscribe(console.onComplete);

    starImgConverter.OnError.Subscribe(console.onError);
    starImgConverter.OnChangeProgress.Subscribe(console.onChangeProgress);
    starImgConverter.OnComplete.Subscribe(console.onComplete);

    console.OnLoad.Subscribe(onLoad);
    console.OnConvert.Subscribe(onConvert);
    console.OnPreview.Subscribe(onPreview);
    console.OnKill.Subscribe(onKill);

    console.Main();

    return EXIT_SUCCESS;
}