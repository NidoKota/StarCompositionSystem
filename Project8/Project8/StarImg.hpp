#pragma once

#include <iostream>
#include <optional>
#include <memory>
#include <opencv2/opencv.hpp>

#include "CvUtility.hpp"

namespace StarCompositionSystem
{
    using namespace std;
    using namespace cv;

    class StarImg
    {
    private:
        shared_ptr<Mat> _img;
        optional<float> _lon = nullopt;
        optional<float> _lat = nullopt;
        optional<int> _lonIndex = nullopt;
        optional<int> _latIndex = nullopt;
        optional<int> _sourceIndex = nullopt;

    public:
        inline void SetLon(const float value)
        {
            _lon = value;
        }

        inline void SetLat(const float value)
        {
            _lat = value;
        }

        inline void SetLonIndex(const int value)
        {
            _lonIndex = value;
        }

        inline void SetLatIndex(const int value)
        {
            _latIndex = value;
        }

        inline void SetSourceIndex(const int value)
        {
            _sourceIndex = value;
        }

        inline void SetImg(Mat* value)
        {
            _img.reset(value);
        }

        inline bool IsValid() const
        {
            return _lon && _lat && _lonIndex && _latIndex && _sourceIndex && _img;
        }

        inline float lon() const
        {
            return _lon.value();
        }

        inline float lat() const
        {
            return _lat.value();
        }

        inline int lonIndex() const
        {
            return _lonIndex.value();
        }

        inline int latIndex() const
        {
            return _latIndex.value();
        }

        inline int sourceIndex() const
        {
            return _sourceIndex.value();
        }

        inline Mat& img() const
        {
            return *_img;
        }

        inline Mat& img()
        {
            return *_img;
        }

        inline friend ostream& operator << (ostream&, const StarImg&);
    };

    inline ostream& operator << (ostream& stream, const StarImg& starImg)
    {
        if (!starImg.IsValid())
        {
            stream << "IsNotValid";
            return stream;
        }

        stream
            << setprecision(3)
            << "("
            << "lon " << setw(4) << starImg.lon()
            << ", "
            << "lonIndex " << setw(2) << starImg.lat()
            << ")";

        return stream;
    }
}