#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <thread>

#include "StarUtility.hpp"
#include "CvUtility.hpp"
#include "StarImg.hpp"
#include "Event.hpp"

namespace StarCompositionSystem
{
    using namespace std;
    using namespace cv;

    class StarImgLoader
    {
    private:
        bool _isRunning;
        Event<void, string> _onErrorEvent;
        Event<void, string> _onCompleteEvent;

    public:
        IReadOnlyEvent<void, string>& OnErrorEvent = _onErrorEvent;
        IReadOnlyEvent<void, string>& OnCompleteEvent = _onCompleteEvent;

    private:
        void LoadInternal(StarImg& starImg, string parentFolderPath, int sourceIndex, size_t lonIndex, size_t latIndex)
        {
            const float lat = StarData::latIndexToLat[latIndex];
            const size_t lonImgCount = StarData::latToLonImgCount.at(lat);
            const float lonStepMinit = StarData::latToLonStepMinit.at(lat);
            const float lonStep = (lonStepMinit / (24 * 60)) * 360;

            const string path = StarData::GetSoucesPath(parentFolderPath, sourceIndex + 1);

            Mat* starMat = new Mat(getImage(path));

            starImg.SetImg(starMat);

            starImg.SetLon(lonIndex * lonStep);
            starImg.SetLat(lat);

            starImg.SetLonIndex(lonIndex);
            starImg.SetLatIndex(latIndex);

            starImg.SetSourceIndex(sourceIndex);
        }

        void LoadSingleThread(vector<StarImg>& result, string parentFolderPath)
        {
            int allImgCount = StarData::GetAllImgCount();

            result = vector<StarImg>(allImgCount);

            int sourceIndex = 0;
            for (size_t latIndex = 0; latIndex < size(StarData::latIndexToLat); latIndex++)
            {
                const float lat = StarData::latIndexToLat[latIndex];
                const size_t lonImgCount = StarData::latToLonImgCount.at(lat);

                for (size_t lonIndex = 0; lonIndex < lonImgCount; lonIndex++, sourceIndex++)
                {
                    if (!_isRunning) break;
                    LoadInternal(result[sourceIndex], parentFolderPath, sourceIndex, lonIndex, latIndex);
                }
            }
        }

        void LoadMultiThread(vector<StarImg>& result, string parentFolderPath)
        {
            int allImgCount = StarData::GetAllImgCount();

            result = vector<StarImg>(allImgCount);
            vector<thread> loadStarImgsThreads = vector<thread>(allImgCount);

            int sourceIndex = 0;
            for (size_t latIndex = 0; latIndex < size(StarData::latIndexToLat); latIndex++)
            {
                const float lat = StarData::latIndexToLat[latIndex];
                const size_t lonImgCount = StarData::latToLonImgCount.at(lat);

                for (size_t lonIndex = 0; lonIndex < lonImgCount; lonIndex++, sourceIndex++)
                {
                    if (!_isRunning) break;
                    loadStarImgsThreads[sourceIndex] =
                        thread([=, &result]
                            {
                                LoadInternal(result[sourceIndex], parentFolderPath, sourceIndex, lonIndex, latIndex);
                            });
                }
            }

            for (thread& th : loadStarImgsThreads)
            {
                th.join();
            }
        }

    public:
        void Load(vector<StarImg>& result, string parentFolderPath, bool multiThread = true)
        {
            if (_isRunning)
            {
                _onErrorEvent("IsRunning");
                return;
            }

            _isRunning = true;
            {
                if (multiThread) LoadMultiThread(result, parentFolderPath);
                else             LoadSingleThread(result, parentFolderPath);
            }
            _isRunning = false;

            _onCompleteEvent("Load " + to_string(result.size()) + " imgs");
        }

        vector<StarImg> Load(string parentFolderPath, bool multiThread = true)
        {
            vector<StarImg> result;
            Load(result, parentFolderPath, multiThread);
            return result;
        }

        void Kill()
        {
            _isRunning = false;
        }
    };
}