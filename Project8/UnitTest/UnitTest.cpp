#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
    using namespace std;
    using namespace cv;
    using namespace StarCompositionSystem;

    //mutex writeMutex;
    inline void writeMessageMultiThread(stringstream& ss)
    {
        //lock_guard<mutex> lock(writeMutex);
        Logger::WriteMessage(ss.str().c_str());
    }

#define TEST_OUT(x) \
do { stringstream ss; ss << getTimeStamp() << " " << boolalpha << x << endl; writeMessageMultiThread(ss); } while(false)

	TEST_CLASS(UnitTest)
	{
	public:
        TEST_METHOD(ShowImageWaitAndThrough)
        {
            Mat src = getImage("C:/img/Lenna256x256.bmp");

            thread th;
            showImageWaitAndThrough(src, th);
            th.join();

            showImageWaitAndThrough(src);
            this_thread::sleep_for(chrono::seconds(2));
        }

        TEST_METHOD(ThreadTestA)
        {
            std::mutex mtx;
            std::condition_variable cv;
            bool isMainReady = false;
            bool isSubReady = false;

            std::thread th([&]()
                {
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    {
                        std::unique_lock<std::mutex> lck(mtx);
                        TEST_OUT("[Sub] start");
                        isSubReady = true;
                        if (isMainReady)
                        {
                            TEST_OUT("[Sub] wakes main up");
                            cv.notify_one();
                        }
                        else
                        {
                            TEST_OUT("[Sub] sleeps until main is ready");
                            while (!isMainReady)
                                cv.wait(lck);
                        }
                    }
                    TEST_OUT("[Sub] started");
                });

            std::this_thread::sleep_for(std::chrono::seconds(2));
            {
                std::unique_lock<std::mutex> lck(mtx);
                TEST_OUT("[Main] start");
                isMainReady = true;
                if (isSubReady)
                {
                    TEST_OUT("[Main] wakes sub up");
                    cv.notify_one();
                }
                else
                {
                    TEST_OUT("[Main] sleeps until sub is ready");
                    while (!isSubReady)
                        cv.wait(lck);
                }
            }
            TEST_OUT("[Main] started");

            th.join();

        }

        TEST_METHOD(ThreadTestB)
        {
            TEST_OUT("ThreadTestB");

            bool notify = false;
            mutex mtx;
            condition_variable cv;

            thread A([&]()
                {
                    TEST_OUT("A");

                    TEST_OUT("A wait");
                    this_thread::sleep_for(chrono::seconds(2));
                    TEST_OUT("A wait end");

                    TEST_OUT("A lock");
                    unique_lock<mutex> lk(mtx);

                    TEST_OUT("A wait");
                    this_thread::sleep_for(chrono::seconds(2));
                    TEST_OUT("A wait end");

                    TEST_OUT("A notify_all");
                    notify = true;
                    cv.notify_all();

                    TEST_OUT("A wait");
                    this_thread::sleep_for(chrono::seconds(2));
                    TEST_OUT("A wait end");

                    TEST_OUT("A unlock");
                    lk.unlock();

                    TEST_OUT("A wait");
                    this_thread::sleep_for(chrono::seconds(2));
                    TEST_OUT("A wait end");

                    TEST_OUT("A End");
                });

            thread B([&]()
                {
                    TEST_OUT("B");
                    TEST_OUT("B lock");
                    unique_lock<mutex> lk(mtx);

                    TEST_OUT("B wait");
                    this_thread::sleep_for(chrono::seconds(4));
                    TEST_OUT("B wait end");

                    TEST_OUT("B cv.wait");
                    cv.wait(lk, [&]
                        {
                            TEST_OUT("B cv check " << notify);
                            return notify;
                        });

                    TEST_OUT("B wait");
                    this_thread::sleep_for(chrono::seconds(2));
                    TEST_OUT("B wait end");

                    TEST_OUT("B End");
                });

            B.join();
            A.join();
        }

        TEST_METHOD(ThreadTestC)
        {
            thread A([&]()
                {
                    this_thread::sleep_for(chrono::seconds(2));
                    TEST_OUT("A End");
                });

            thread B([&]()
                {
                    TEST_OUT("B A" << A.joinable());

                    A.join();

                    this_thread::sleep_for(chrono::seconds(4));
                    TEST_OUT("B End");
                });

            TEST_OUT("B" << B.joinable());
            TEST_OUT("A" << A.joinable());

            B.join();
        }

		TEST_METHOD(Perspective)
		{
            Mat src = getImage("C:/img/Lenna256x256.bmp");

            showImage(src);
            Mat bg = src.clone();

            for (size_t i = 0; i < 10; i++)
            {
                float progress = (i + 1) / 10.0f;
                float invPprogress = 1 - progress;
                Mat dir = perspective(src, Point2f(0, 0), Point2f(256, 0), Point2f(0, 256), Point2f(256, 256 * 2 * progress));
                showImage(dir);
                waitKey(0);
            }
		}

        TEST_METHOD(Affine)
        {
            Mat src = getImage("C:/img/Lenna256x256.bmp");

            showImage(src);
            Mat bg = src.clone();

            for (size_t i = 0; i < 10; i++)
            {
                float progress = (i + 1) / 10.0f;
                float invPprogress = 1 - progress;
                Mat dir = affine(src, Point2f(0, 0), Point2f(256, 256 * 2 * progress));
                showImage(dir);

                waitKey(0);
            }
        }

        TEST_METHOD(AccessToPixel)
        {
            Mat src = getImage("C:/img/Lenna256x256.bmp");

            showImage(src);
            Mat src2 = src.clone();
            Point2i src2Size = src2.size();

            AccessToPixelFunc accessToPixelFunc = [=](int x, int y, int ch, int index, uchar val)
                {
                    Point2f uv = Point2f((float)x / src2Size.x, (float)y / src2Size.y);
                    float data = val / 255.0f;

                    data = uv.x * data + (1 - uv.x) * 1;

                    return data * 255;
                };

            accessToPixel(src2, accessToPixelFunc);
            showImage(src2);

            waitKey(0);
        }

        TEST_METHOD(SplitScale)
        {
            Mat src = getImage("C:/img/Lenna256x256.bmp");

            showImage(src);
            Mat src2 = src.clone();
            Mat bg = Mat(src2.size() + Size(200, 200), src2.type());

            SplitScalerFunc scalerFunc1 = [src2](int line) -> float
            {
                float th = ((float)line / src2.rows) * 2 * M_PI;
                return 0.8f + cos(th) * 0.2f;
            };

            splitScale(bg, src2, SplitScalerDirection::X, scalerFunc1);
            addText(bg, "x", 4);
            showImage(bg);

            SplitScalerFunc scalerFunc2 = [src2](int line) -> float
            {
                return ((float)line / src2.cols);
            };

            Mat result2 = splitScale(src2, SplitScalerDirection::Y, scalerFunc2);
            result2 = addText(result2, "y", 4);
            showImage(result2);

            waitKey(0);
        }

        TEST_METHOD(MatchTemplate)
        {
#define THRESHOLD 0.9
#define FIND_TARGET_NUM 30

            Mat src = getImage("C:/img/Lenna256x256.bmp");
            Mat tmp = imread("C:/img/Lenna256x256_mini2.bmp", 1);

            Mat result;
            matchTemplate(src, tmp, result, TemplateMatchModes::TM_CCOEFF_NORMED);

            double val;
            Point loc;
            minMaxLoc(result, NULL, &val, NULL, &loc);
            showImage(result);

            if (abs(val) > THRESHOLD) rectangle(src, Point(loc.x, loc.y), Point(loc.x + tmp.cols, loc.y + tmp.rows), Scalar(0, 0, 255), 2);

            showImage(src);

            waitKey(0);
        }

        TEST_METHOD(Rot)
        {
            Mat src = getImage("C:/img/Lenna256x256.bmp");
            Mat tmp = getImage("C:/img/Lenna256x256_mini2.bmp");

            Point2i srcSize = src.size();

            float rangeTheta = atan2(srcSize.y / 2.0f, srcSize.x / 2.0f);
            float range = srcSize.x / cos(rangeTheta);

            int rangeInt = ceil(range);
            if (rangeInt % 2 != 0) rangeInt++;

            Mat result = Mat(Size(rangeInt, rangeInt), CV_8UC3);

            showImage(rotation(result.clone(), src, 30));
            showImage(rotation(result.clone(), src, 60));
            showImage(rotation(result.clone(), src, 90));
            showImage(rotation(result.clone(), src, 120));

            waitKey(0);
        }

        TEST_METHOD(RotMatchTemplate)
        {
            Mat src = getImage("C:/img/Lenna256x256.bmp");
            Mat tmp = getImage("C:/img/LennaRot.bmp");
            //Mat tmp = getImage("C:/img/LennaFailed.bmp");

            Point2i srcSize = src.size();

            float rangeTheta = atan2(srcSize.y / 2.0f, srcSize.x / 2.0f);
            float range = srcSize.x / cos(rangeTheta);

            int rangeInt = ceil(range);

            Mat rotatedSrc = Mat(Size(rangeInt, rangeInt), CV_8UC3);
            Mat result;

            int direction = +1;
            int degree = 0;
            for (size_t i = 0; i < 180; )
            {
                degree = i * direction;
                TEST_OUT("check" << degree << " degree");

                Mat _rotatedSrc = rotation(rotatedSrc.clone(), src, degree);

                matchTemplate(_rotatedSrc, tmp, result, TemplateMatchModes::TM_CCOEFF_NORMED);

                double val;
                Point loc;
                minMaxLoc(result, NULL, &val, NULL, &loc);

                if (abs(val) > THRESHOLD)
                {
                    rectangle(_rotatedSrc, Point(loc.x, loc.y), Point(loc.x + tmp.cols, loc.y + tmp.rows), Scalar(0, 0, 255), 2);
                    showImage(_rotatedSrc);
                    TEST_OUT("find" << degree << " degree");
                    waitKey(0);
                    break;
                }

                direction *= -1;
                if (direction == +1) i++;
            }
        }
	};
}
