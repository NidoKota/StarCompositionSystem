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
using namespace StarCompositionSystem;

StarImgLoader starImgLoader = StarImgLoader();
StarImgConverter starImgConverter = StarImgConverter();
vector<StarImg> starImgs;
Mat result;

int main(int argc, char* argv[])
{
    Console console(argv[0]);

    starImgLoader.OnErrorEvent.Subscribe(bind(&Console::OnError, &console, placeholders::_1));
    starImgLoader.OnCompleteEvent.Subscribe(bind(&Console::OnComplete, &console, placeholders::_1));

    starImgConverter.OnErrorEvent.Subscribe(bind(&Console::OnError, &console, placeholders::_1));
    starImgConverter.OnChangeProgressEvent.Subscribe(bind(&Console::OnChangeProgress, &console, placeholders::_1));
    starImgConverter.OnCompleteEvent.Subscribe(bind(&Console::OnComplete, &console, placeholders::_1));

    console.OnLoadEvent.Subscribe([&](tuple<string, bool> args)
        {
            auto& [loadPath, multiThread] = args;
            starImgLoader.Load(starImgs, loadPath, multiThread);
        });

    console.OnConvertEvent.Subscribe([&](bool multiThread)
        {
            starImgConverter.Convert(result, starImgs, multiThread);
            saveImage(result);
        });

    console.OnPreviewEvent.Subscribe([&](tuple<int, string> args)
        {
            auto& [sourceIndex, endPassName] = args;
            Mat m = starImgConverter.SingleConvert(starImgs, sourceIndex, endPassName);
            showImageWaitAndThrough(m);
        });

    console.OnKillEvent.Subscribe([&]()
        {
            starImgLoader.Kill();
            starImgConverter.Kill();
        });

    console.Main();

    return EXIT_SUCCESS;
}