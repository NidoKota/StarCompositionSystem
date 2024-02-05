#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
#include <optional>
#include <tuple>

namespace StarCompositionSystem
{
    using namespace std;

    class PassBaseNonGeneric
    {
    public:
        virtual string GetName() = 0;

        PassBaseNonGeneric() {}
        virtual ~PassBaseNonGeneric() {}
    };

    template<class TInput, class TOutput>
    class PassBase : public PassBaseNonGeneric
    {
    public:
        virtual string GetName() override = 0;
        virtual TOutput Calcurate(Mat& inout, TInput input) = 0;

        PassBase() {}
        virtual ~PassBase() override {}
    };

    template<class TInput>
    class PassBase<TInput, void> : public PassBaseNonGeneric
    {
    public:
        virtual string GetName() override = 0;
        virtual void Calcurate(Mat& inout, TInput input) = 0;

        PassBase() {}
        virtual ~PassBase() override {}
    };

    template<class TOutput>
    class PassBase<void, TOutput> : public PassBaseNonGeneric
    {
    public:
        virtual string GetName() override = 0;
        virtual TOutput Calcurate(Mat& inout) = 0;

        PassBase() {}
        virtual ~PassBase() override {}
    };

    template<>
    class PassBase<void, void> : public PassBaseNonGeneric
    {
    public:
        virtual string GetName() override = 0;
        virtual void Calcurate(Mat& inout) = 0;

        PassBase() {}
        virtual ~PassBase() override {}
    };
}