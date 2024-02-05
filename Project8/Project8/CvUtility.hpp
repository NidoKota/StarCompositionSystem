#pragma once

#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <opencv2/opencv.hpp>
#include <mutex>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <optional>

#pragma warning(disable : 4996)

namespace StarCompositionSystem
{
    using namespace std;
    using namespace cv;

    //mutex coutMutex;
    inline void coutMultiThread(stringstream& ss)
    {
        //lock_guard<mutex> lock(coutMutex);
        cout << ss.str();
    }

    inline string getTimeStamp()
    {
        auto now = chrono::system_clock::now();
        auto nowC = chrono::system_clock::to_time_t(now);
        auto nowMs = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;
        stringstream ss;
        ss << put_time(localtime(&nowC), "%T") << '.' << setfill('0') << setw(3) << nowMs.count();
        return ss.str();
    }

#define OUT(x) \
do { stringstream ss; ss << getTimeStamp() << " " << boolalpha << x << endl; coutMultiThread(ss); } while (false)

#define ERROR_OUT(x) \
do { stringstream ss; ss << "error! " << getTimeStamp() << " " << boolalpha << x << endl; coutMultiThread(ss); } while (false)

#define PUBLIC_GET_PRIVATE_SET(type, name) \
public: type Get##name(){ return name; } \
private: type name;

#define NAMEOF(x)\
#x

    inline String saveImage(const Mat& image, const String& filename = nullptr)
    {
        static int saveIndex = 0;

        String result = filename;
        if (result.empty()) result = to_string(saveIndex);
        saveIndex++;

        imwrite("C:/img/" + filename + ".png", image);

        return result;
    }

    inline String showImage(const Mat& image, const String& winname = nullptr)
    {
        static int windowIndex = 0;

        String result = winname;
        if (result.empty()) result = to_string(windowIndex);
        windowIndex++;

        namedWindow(result, WindowFlags::WINDOW_AUTOSIZE);
        imshow(result, image);
        moveWindow(result, 0, 0);

        return result;
    }

    inline String showImageWaitAndThrough(const Mat& image, const String& winname = nullptr)
    {
        bool endShow = false;
        String result = nullptr;

        thread th([&result, &image, &winname, &endShow]()
            {
                result = showImage(image, winname);
                endShow = true;
                waitKey(0);
            });

        while (!endShow)
        {
            this_thread::yield();
        }

        th.detach();

        return result;
    }

    inline String showImageWaitAndThrough(const Mat& image, thread& outThread, const String& winname = nullptr)
    {
        bool endShow = false;
        String result = nullptr;

        outThread = thread([&result, &image, &winname, &endShow]()
            {
                result = showImage(image, winname);
                endShow = true;
                waitKey(0);
            });

        while (!endShow) 
        {
            this_thread::yield();
        }

        return result;
    }

    inline Mat getImage(const String& path)
    {
        Mat result = imread(path, 1);

        if (!result.empty()) return result;

        ERROR_OUT("file open error");
        exit(EXIT_FAILURE);
    }

    inline Mat affine(const Mat& src, const Point2f& leftUp, const Point2f& rightDown)
    {
        Mat result;

        const vector<Point2f> srcVec = { Point2f(0, 0),  Point2f(src.cols, 0), Point2f(src.cols, src.rows) };
        const vector<Point2f> dstVec = { leftUp, Point2f(rightDown.x, leftUp.y), rightDown };

        Mat mat = getAffineTransform(srcVec, dstVec);
        warpAffine(src, result, mat, result.size(), InterpolationFlags::INTER_LINEAR, BorderTypes::BORDER_TRANSPARENT);

        return result;
    }

    inline Mat affine(const Mat& bg, const Mat& src, const Point2f& leftUp, const Point2f& rightDown)
    {
        const vector<Point2f> srcVec = { Point2f(0, 0),  Point2f(src.cols, 0), Point2f(src.cols, src.rows) };
        const vector<Point2f> dstVec = { leftUp, Point2f(rightDown.x, leftUp.y), rightDown };

        Mat mat = getAffineTransform(srcVec, dstVec);
        warpAffine(src, bg, mat, bg.size(), InterpolationFlags::INTER_LINEAR, BorderTypes::BORDER_TRANSPARENT);

        return bg;
    }

    inline Mat perspective(const Mat& src, const Point2f& leftUp, const Point2f& rightUp, const Point2f& leftDown, const Point2f& rightDown)
    {
        Mat result;

        const vector<Point2f> srcVec = { Point2f(0, 0),  Point2f(src.cols, 0), Point2f(0, src.rows), Point2f(src.cols, src.rows) };
        const vector<Point2f> dstVec = { leftUp, rightUp, leftDown, rightDown };

        Mat mat = getPerspectiveTransform(srcVec, dstVec);
        warpPerspective(src, result, mat, result.size(), InterpolationFlags::INTER_LINEAR, BorderTypes::BORDER_TRANSPARENT);

        return result;
    }

    inline Mat perspective(const Mat& bg, const Mat& src, const Point2f& leftUp, const Point2f& rightUp, const Point2f& leftDown, const Point2f& rightDown)
    {
        const vector<Point2f> srcVec = { Point2f(0, 0),  Point2f(src.cols, 0), Point2f(0, src.rows), Point2f(src.cols, src.rows) };
        const vector<Point2f> dstVec = { leftUp, rightUp, leftDown, rightDown };

        Mat mat = getPerspectiveTransform(srcVec, dstVec);
        warpPerspective(src, bg, mat, bg.size(), InterpolationFlags::INTER_LINEAR, BorderTypes::BORDER_TRANSPARENT);

        return bg;
    }

    inline Mat rotation(const Mat& src, const float& degree, const optional<Point2f> center = nullopt)
    {
        Mat result;

        Point2f _center = center ? center.value() : Point2f(src.cols / 2.0f, src.rows / 2.0f);

        Mat mat = getRotationMatrix2D(_center, degree, 1);
        warpAffine(src, result, mat, result.size(), InterpolationFlags::INTER_LINEAR, BorderTypes::BORDER_TRANSPARENT);

        return result;
    }

    inline Mat rotation(const Mat& bg, const Mat& src, const float& degree, const optional<Point2f> bgCenter = nullopt/*const optional<Point2f> srcCenter = nullopt*/)
    {
        //TODO: fix srcCenter not null bug

        Point2f _bgCenter = bgCenter ? bgCenter.value() : Point2f(bg.cols / 2.0f, bg.rows / 2.0f);
        Point2f _srcCenter = /*srcCenter ? srcCenter.value() : */Point2f(src.cols / 2.0f, src.rows / 2.0f);
        Point2f moveVector = _bgCenter - _srcCenter;

        const vector<Point2f> srcVec = { Point2f(0, 0),  Point2f(src.cols, 0), Point2f(src.cols, src.rows) };
        const vector<Point2f> dstVec = { srcVec[0] + moveVector, srcVec[1] + moveVector, srcVec[2] + moveVector };

        Mat movMat = getAffineTransform(srcVec, dstVec);
        Mat rotMat = getRotationMatrix2D(_bgCenter, degree, 1);

        Mat m = (cv::Mat_<double>(1, 3) << 0, 0, 1);
        movMat.push_back(m);
        rotMat.push_back(m);

        warpPerspective(src, bg, rotMat * movMat, bg.size(), InterpolationFlags::INTER_LINEAR, BorderTypes::BORDER_TRANSPARENT);

        return bg;
    }

    inline Mat cutout(const Mat& src, const Point2f& leftUp, const Point2f& rightDown)
    {
        float left = leftUp.x;
        float up = leftUp.y;

        float right = rightDown.x;
        float down = rightDown.y;

        float width = right - left;
        float height = down - up;

        return Mat(src, Rect(left, up, width, height));
    }

    using SplitScalerFunc = function<float(int line)>;

    enum SplitScalerDirection
    {
        X,
        Y
    };

    inline Mat splitScale(const Mat& bg, const Mat& src, const SplitScalerDirection& direction, const SplitScalerFunc& scalerFunc)
    {
        using AccessFunc = Mat(Mat::*)(int line) const;

        int lineCount = direction == SplitScalerDirection::X ? src.rows : src.cols;

        AccessFunc accessFunc = direction == SplitScalerDirection::X ? &Mat::row : &Mat::col;
        Size splitSizeExpander = direction == SplitScalerDirection::X ? Size(0, 1) : Size(1, 0);

        for (int line = 0; line < lineCount; ++line)
        {
            Mat split = (src.*accessFunc)(line);
            Mat splitCloneExpand = Mat(split.size() + splitSizeExpander, split.type());

            split.copyTo((splitCloneExpand.*accessFunc)(0));

            float scaler = scalerFunc(line);

            Point2f resultCenter = ((Point2f)bg.size()) / 2.0f;
            Point2f scaledResultScale = ((Point2f)src.size()) * scaler;
            float halfLineCount = lineCount / 2.0f;

            Point2f lu;
            Point2f rd;

            if (direction == SplitScalerDirection::X)
            {
                lu = Point2f(resultCenter.x - scaledResultScale.x / 2.0f, resultCenter.y - halfLineCount + line);
                rd = Point2f(resultCenter.x + scaledResultScale.x / 2.0f, resultCenter.y - halfLineCount + line + 1);
            }
            else
            {
                lu = Point2f(resultCenter.x - halfLineCount + line, resultCenter.y - scaledResultScale.y / 2.0f);
                rd = Point2f(resultCenter.x - halfLineCount + line + 1, resultCenter.y + scaledResultScale.y / 2.0f);
            }

            affine(bg, splitCloneExpand, lu, rd);
        }

        return bg;
    }

    inline Mat splitScale(const Mat& src, const SplitScalerDirection& direction, const SplitScalerFunc& scalerFunc)
    {
        return splitScale(Mat(src.size(), src.type()), src, direction, scalerFunc);
    }

    inline vector<string> split(const string& str, const char& targetCh, const int& targetCount = INT_MAX)
    {
        vector<string> result;
        string item;

        int cou = 0;

        for (char ch : str)
        {
            cou++;

            if (ch == targetCh) cou = targetCount;
            else item += ch;

            if (cou >= targetCount)
            {
                if (!item.empty()) result.push_back(item);

                item.clear();
                cou = 0;
            }
        }

        if (!item.empty()) result.push_back(item);

        return result;
    }

    inline Mat addTextThickness(const Mat& src, const String& text, const float& fontSize, const int& thickness, const Point2f& addPoint = Point2f(0, 0), const Scalar& color = Scalar(255, 255, 255))
    {
        int baseLine;
        Size size = getTextSize(text, HersheyFonts::FONT_HERSHEY_PLAIN, fontSize, thickness, &baseLine);
        putText(src, text, Point2f(addPoint.x, fontSize * 10 + addPoint.y), HersheyFonts::FONT_HERSHEY_PLAIN, fontSize, color, thickness);

        return src;
    }

    inline Mat addText(const Mat& src, const String& text, const float& fontSize, const Point2f& addPoint = Point2f(0, 0), const Scalar& color = Scalar(255, 255, 255))
    {
        return addTextThickness(src, text, fontSize, fontSize, addPoint, color);
    }

    inline Mat addSplitTextThickness(const Mat& src, const string& text, const int& maxCount, const float& fontSize, const int& thickness, const Point2f& addPoint = Point2f(0, 0), const Scalar& color = Scalar(255, 255, 255))
    {
        Mat result = src.clone();

        vector<string> elems = split(text, '\n', maxCount);

        int index = 0;
        for (string elem : elems)
        {
            result = addTextThickness(result, elem, fontSize, thickness, Point2f(0, index * (fontSize * 10 + 5)) + addPoint, color);
            index++;
        }

        return result;
    }

    inline Mat addSplitText(const Mat& src, const string& text, int& maxCount, const float& fontSize, const Point2f& addPoint = Point2f(0, 0), const Scalar& color = Scalar(255, 255, 255))
    {
        return addSplitTextThickness(src, text, maxCount, fontSize, fontSize, addPoint, color);
    }

    using AccessToPixelFunc = function<uchar(int x, int y, int ch, int index, uchar val)>;

    inline void accessToPixel(const Mat& image, const AccessToPixelFunc& func)
    {
        for (int x = 0; x < image.cols; ++x)
        {
            for (int y = 0; y < image.rows; ++y)
            {
                for (int ch = 0; ch < image.channels(); ++ch)
                {
                    int index = x * (int)image.elemSize() + y * (int)image.step + ch;

                    uchar value = func(x, y, ch, index, image.data[index]);
                    image.data[index] = value;
                }
            }
        }
    }

    inline Point2f scale(const Point2f& l, const Point2f& r)
    {
        return Point2f(l.x * r.x, l.y * r.y);
    }

    inline Point2f divide(const Point2f& l, const Point2f& r)
    {
        return Point2f(l.x / r.x, l.y / r.y);
    }

    [[deprecated]]
    inline Point2f convertRightUpCenter01ToCvScape(const Point2f& v, const Point2f& size)
    {
        Point2f rightDownCenter01 = Point2f(v.x, -v.y);
        Point2f rightDown01 = Point2f((rightDownCenter01.x + 1) / 2, (rightDownCenter01.y + 1) / 2);
        return Point2f(rightDown01.x * size.x, rightDown01.y * size.y);
    }

    template <class _T>
    inline _T clamp(const _T& value, const _T& min, const _T& max)
    {
        return value < min ? min : value > max ? max : value;
    }

    template <>
    inline Point2f clamp(const Point2f& value, const Point2f& min, const Point2f& max)
    {
        return Point2f(
            value.x < min.x ? min.x : value.x > max.x ? max.x : value.x,
            value.y < min.y ? min.y : value.y > max.y ? max.y : value.y);
    }

    template <class _T>
    inline _T remap(const _T& value, const _T& inMin, const _T& inMax, const _T& outMin, const _T& outMax)
    {
        return clamp(outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin), outMin, outMax);
    }
}