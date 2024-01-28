#pragma once

#define _USE_MATH_DEFINES

#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <thread>

#include "StarImg.hpp"
#include "StarUtility.hpp"
#include "CvUtility.hpp"
#include "Event.hpp"
#include "SideStarImg.hpp"

#include "PassBase.hpp"
#include "RemoveIndex0ImgPass.hpp"
#include "CalcRotationPass.hpp"
#include "CalcScalePass.hpp"
#include "ScaledImgPass.hpp"
#include "CutoutImgPass.hpp"
#include "CompositePass.hpp"

using namespace std;
using namespace cv;

class StarImgConverter
{
    class IsKilledException : Exception { };

private:
    Event<void, string> _onErrorEvent;
    Event<void, string> _onPassInitializeEvent;
    Event<void, string> _onPassFinalizeEvent;
    Event<void, tuple<int, int, float>> _onChangeProgressEvent;
    Event<void, string> _onCompleteEvent;

    RemoveIndex0ImgPass _removeIndex0ImgPass = RemoveIndex0ImgPass();
    CalcRotationPass _calcRotationPass = CalcRotationPass();
    CalcScalePass _calcScalePass = CalcScalePass();
    ScaledImgPass _scaledImgPass = ScaledImgPass();
    CutoutImgPass _cutoutImgPass = CutoutImgPass();
    CompositePass _compositePass = CompositePass();
    float _progress;
    bool _isRunning;

public:
    IReadOnlyEvent<void, string>& OnErrorEvent = _onErrorEvent;
    IReadOnlyEvent<void, string>& OnPassInitializeEvent = _onPassInitializeEvent;
    IReadOnlyEvent<void, string>& OnPassFinalizeEvent = _onPassFinalizeEvent;
    IReadOnlyEvent<void, tuple<int, int, float>>& OnChangeProgressEvent = _onChangeProgressEvent;
    IReadOnlyEvent<void, string>& OnCompleteEvent = _onCompleteEvent;

private:
    void PassInitialize(PassBaseNonGeneric& passBase, optional<string> endPassName)
    {
        if (!_isRunning) throw IsKilledException();

        string passName = passBase.GetName();
        _onPassInitializeEvent(passName);

        bool isEndPassNameNull = !endPassName || endPassName.value().empty();
        if (!isEndPassNameNull && passName == endPassName) throw IsKilledException();
    }

    void PassFinalize(PassBaseNonGeneric& passBase)
    {
        if (!_isRunning) throw IsKilledException();

        string passName = passBase.GetName();
        _onPassFinalizeEvent(passName);
    }

    float Progress()
    {
        return _progress;
    }

    void SetProgress(int step, int allStep, float value)
    {
        _progress = value;
        _onChangeProgressEvent({ step, allStep, value });
    }

    void ConvertInternal(Mat& result, SideStarImg& sideStarImg, Mat& img, optional<string> endPassName = nullopt)
    {
        StarImg& starImg = sideStarImg.center;
        img = starImg.img();

        try
        {
            PassInitialize(_removeIndex0ImgPass, endPassName);
            bool isRemoved = _removeIndex0ImgPass.Calcurate(img, starImg);
            PassFinalize(_removeIndex0ImgPass);

            if (!isRemoved) 
            {
                PassInitialize(_calcRotationPass, endPassName);
                _calcRotationPass.Calcurate(img, sideStarImg);
                PassFinalize(_calcRotationPass);

                PassInitialize(_calcScalePass, endPassName);
                StarScale starScale = _calcScalePass.Calcurate(img, starImg);
                PassFinalize(_calcScalePass);

                PassInitialize(_scaledImgPass, endPassName);
                _scaledImgPass.Calcurate(img, starScale);
                PassFinalize(_scaledImgPass);

                PassInitialize(_cutoutImgPass, endPassName);
                _cutoutImgPass.Calcurate(img, { starImg, starScale });
                PassFinalize(_cutoutImgPass);
            }

            PassInitialize(_compositePass, endPassName);
            _compositePass.Calcurate(img, { starImg, result });
            PassFinalize(_compositePass);
        }
        catch (IsKilledException e)
        {

        }
    }

    void ConvertMultiThread(Mat& result, vector<StarImg>& starImgs, optional<string> endPassName = nullopt)
    {
        float progressDelta = 1.0f / starImgs.size();

        vector<thread> convertThreads = vector<thread>(starImgs.size());
        vector<Mat> imgs = vector<Mat>(starImgs.size());

        for (size_t sourceIndex = 0; sourceIndex < starImgs.size(); sourceIndex++)
        {
            if (!_isRunning) break;

            convertThreads[sourceIndex] =
                thread([=, &result, &imgs, &starImgs]
                    {
                        SideStarImg sideStarImg;

                        GetSideStarImg(sideStarImg, starImgs, sourceIndex);
                        ConvertInternal(result, sideStarImg, imgs[sourceIndex]);
                        SetProgress(1, 1, Progress() + progressDelta);
                    });
        }

        for (thread& th : convertThreads)
        {
            if (th.joinable()) th.join();
        }
    }

    void GetSideStarImg(SideStarImg& result, vector<StarImg>& starImgs, size_t sourceIndex)
    {
        bool add = starImgs.size() - 1 > sourceIndex;
        bool sub = 0 + 1 <= sourceIndex;

        result.center = starImgs[sourceIndex];
        if (add && starImgs[sourceIndex + 1].latIndex() == starImgs[sourceIndex].latIndex()) result.left = starImgs[sourceIndex + 1];
        if (sub && starImgs[sourceIndex - 1].latIndex() == starImgs[sourceIndex].latIndex()) result.right = starImgs[sourceIndex - 1];
    }

    void ConvertSingleThread(Mat& result, vector<StarImg>& starImgs, optional<string> endPassName = nullopt)
    {
        float progressDelta = 1.0f / starImgs.size();

        vector<Mat> imgs = vector<Mat>(starImgs.size());

        for (size_t sourceIndex = 0; sourceIndex < starImgs.size(); sourceIndex++)
        {
            if (!_isRunning) break;

            SideStarImg sideStarImg;

            GetSideStarImg(sideStarImg, starImgs, sourceIndex);
            ConvertInternal(result, sideStarImg, imgs[sourceIndex]);
            SetProgress(1, 1, Progress() + progressDelta);
        }
    }

public:
    void Convert(Mat& result, vector<StarImg>& starImgs, bool multiThread = true, optional<string> endPassName = nullopt)
    {
        if (_isRunning)
        {
            _onErrorEvent("IsRunning");
            return;
        }

        if (starImgs.size() <= 0)
        {
            _onErrorEvent("StarImgsSizeIsZero");
            return;
        }

        _isRunning = true;
        {
            SetProgress(1, 1, 0);

            Point2i imgSize(14400, 3800);
            result = Mat(imgSize.y, imgSize.x, CV_8UC3);

            vector<Mat> imgs;

            if (multiThread) ConvertMultiThread(result, starImgs, endPassName);
            else             ConvertSingleThread(result, starImgs, endPassName);
        }
        _isRunning = false;

        _onCompleteEvent("Convert");
    }

    Mat Convert(vector<StarImg>& starImgs, bool multiThread = true)
    {
        Mat result;
        Convert(result, starImgs, multiThread);
        return result;
    }

    void SingleConvert(Mat& result, vector<StarImg>& starImgs, size_t sourceIndex, optional<string> endPassName = nullopt)
    {
        if (_isRunning)
        {
            _onErrorEvent("IsRunning");
            return;
        }

        _isRunning = true; 
        {
            bool isEndPassNameNull = !endPassName || endPassName.value().empty();
            if (isEndPassNameNull) endPassName = _compositePass.GetName();

            Mat tmp;
            SideStarImg sideStarImg;

            GetSideStarImg(sideStarImg, starImgs, sourceIndex);
            ConvertInternal(tmp, sideStarImg, result, endPassName);
        }
        _isRunning = false;
    }

    Mat SingleConvert(vector<StarImg>& starImgs, size_t sourceIndex, optional<string> endPassName = nullopt)
    {
        Mat result;
        SingleConvert(result, starImgs, sourceIndex, endPassName);
        return result;
    }

    void Kill()
    {
        _isRunning = false;
    }

    tuple<String, String> ShowDebugImg(const StarImg& starImg)
    {
        Mat img = AddDebugText(starImg);
        Mat cimg = AddDebugText(starImg, "converted");

        return { showImage(img), showImage(cimg) };
    }
};