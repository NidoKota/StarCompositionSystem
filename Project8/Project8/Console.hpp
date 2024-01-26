#pragma once

#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <optional>
#include <tuple>

#include <opencv2/opencv.hpp>
#include "cxxopts.hpp"

#include "StarImg.hpp"
#include "StarImgLoader.hpp"
#include "StarImgConverter.hpp"
#include "CvUtility.hpp"
#include "Event.hpp"

using namespace std;
using namespace cv;

class Console
{
private:
    char* _argv0;
    Event<void, tuple<string, bool>> _onLoad;
    Event<void, bool> _onConvert;
    Event<void, tuple<int, string>> _onPreview;
    Event<void, void> _onKill;

public:
    IReadOnlyEvent<void, tuple<string, bool>>& OnLoad = _onLoad;
    IReadOnlyEvent<void, bool>& OnConvert = _onConvert;
    IReadOnlyEvent<void, tuple<int, string>>& OnPreview = _onPreview;
    IReadOnlyEvent<void, void>& OnKill = _onKill;

private:
#define LOAD_MULTI_THREAD_CMD "lmt"
#define LOAD_MULTI_THREAD_EXP "load multi thread"
#define LOAD_PATH_CMD "lpt"
#define LOAD_PATH_EXP "load path"
#define CONVERT_MULTI_THREAD_CMD "cmt"
#define CONVERT_MULTI_THREAD_EXP "convert multi thread"
#define PREVIEW_SOURCE_INDEX_CMD "psi"
#define PREVIEW_SOURCE_INDEX_EXP "preview source index"
#define PREVIEW_END_PASS_NAME_CMD "pep"
#define PREVIEW_END_PASS_NAME_EXP "preview end pass name"

    cxxopts::Options GetOptions(char* argv0)
    {
        using namespace cxxopts;

        Options options(argv0, "Star Composition System");

        options
            .custom_help("[Enter command after booting...]");

        options
            .allow_unrecognised_options()
            .add_options()
            ("h,help",                     "show help")
            ("k",                          "kill command")
            ("l",                          "load imgs")
            (LOAD_MULTI_THREAD_CMD,        LOAD_MULTI_THREAD_EXP,        cxxopts::value<bool>()->default_value("true"))
            (LOAD_PATH_CMD,                LOAD_PATH_EXP,                cxxopts::value<string>()->default_value(""))
            ("c",                          "convert imgs")
            (CONVERT_MULTI_THREAD_CMD,     CONVERT_MULTI_THREAD_EXP,     cxxopts::value<bool>()->default_value("true"))
            ("p",                          "preview img")
            (PREVIEW_SOURCE_INDEX_CMD,     PREVIEW_SOURCE_INDEX_EXP,     cxxopts::value<int>()->default_value("0"))
            (PREVIEW_END_PASS_NAME_CMD,    PREVIEW_END_PASS_NAME_EXP,    cxxopts::value<string>()->default_value(""))
            ;

        return options;
    }

    void OnMassage(string command)
    {
        using namespace cxxopts;

        try 
        {
            bool match = false;
            ParseResult result = GetResult(command);

            ///////////////////////////////////////////////////////////////////////////////////////////////////

            if (result.count("h"))
            {
                OnHelp(_options.value().help());
                match = true;
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////

            if (result.count("k"))
            {
                onError("OnMassageIsNotRunning");
                match = true;
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////

            if (result.count(LOAD_MULTI_THREAD_CMD))
            {
                OnChangeParams({ LOAD_MULTI_THREAD_EXP, result[LOAD_MULTI_THREAD_CMD].as<bool>() ? "true" : "false" });
                match = true;
            }
            if (result.count(LOAD_PATH_CMD))
            {
                OnChangeParams({ LOAD_PATH_EXP , result[LOAD_PATH_CMD].as<string>() });
                match = true;
            }
            if (result.count("l"))
            {
                _onLoad({ result[LOAD_PATH_CMD].as<string>(), result[LOAD_MULTI_THREAD_CMD].as<bool>() });
                match = true;
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////

            if (result.count(CONVERT_MULTI_THREAD_CMD))
            {
                OnChangeParams({ CONVERT_MULTI_THREAD_EXP, result[CONVERT_MULTI_THREAD_CMD].as<bool>() ? "true" : "false" });
                match = true;
            }
            if (result.count("c"))
            {
                _onConvert(result[CONVERT_MULTI_THREAD_CMD].as<bool>());
                match = true;
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////

            if (result.count(PREVIEW_SOURCE_INDEX_CMD))
            {
                OnChangeParams({ PREVIEW_SOURCE_INDEX_EXP, to_string(result[PREVIEW_SOURCE_INDEX_CMD].as<int>()) });
                match = true;
            }
            if (result.count(PREVIEW_END_PASS_NAME_CMD))
            {
                OnChangeParams({ PREVIEW_END_PASS_NAME_EXP, result[PREVIEW_END_PASS_NAME_CMD].as<string>() });
                match = true;
            }
            if (result.count("p"))
            {
                _onPreview({ result[PREVIEW_SOURCE_INDEX_CMD].as<int>(), result[PREVIEW_END_PASS_NAME_CMD].as<string>() });
                match = true;
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////

            if (!match)
            {
                onError("NotMuch");
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////
        }
        catch (OptionException e) 
        {
            onError(e.what());
        }

        _massageThread.value().detach();
        _massageThread = nullopt;

        OnReady();
    }

    void OnMassageKill(string command)
    {
        using namespace cxxopts;

        try
        {
            ParseResult result = GetResult(command);

            if (result.count("k"))
            {
                _onKill();
            }
            else
            {
                onError("OnMassageIsRunning");
            }
        }
        catch (OptionException e)
        {
            onError(e.what());
        }

        _massageKillThread.value().detach();
        _massageKillThread = nullopt;
    }

    ////////////////////////////////////////////////////////////////////////////////////
    
    optional<thread> _massageThread = nullopt;
    optional<thread> _massageKillThread = nullopt;
    optional<cxxopts::Options> _options = nullopt;

    cxxopts::ParseResult GetResult(string command)
    {
        using namespace cxxopts;

        vector<string> commands = split(command, ' ');
        commands.insert(commands.begin(), "");

        int argc = commands.size();
        char** argv = new char* [argc];
        for (int i = 0; i < argc; i++)
        {
            argv[i] = new char[commands[i].size() + 1];
            strcpy(argv[i], commands[i].c_str());
        }

        ParseResult result = _options.value().parse(argc, argv);

        for (int i = 0; i < argc; i++) delete[] argv[i];
        delete[] argv;

        return result;
    }

    void OnReady()
    {
        OUT("Ready...");
    }

    void OnHelp(string helpMessage)
    {
        OUT(helpMessage);
    }

    void OnChangeParams(tuple<string, string> args)
    {
        auto& [exp, value] = args;
        OUT(exp << "=" << value);
    }

public:
    Console(char* argv0)
	{
        _argv0 = argv0;
	}

    void Main() 
    {
        _options = GetOptions(_argv0);
        OUT(_options.value().help());

        OnReady();

        while (true)
        {
            string input;
            getline(cin, input);

            if (_massageThread) _massageKillThread = thread(&Console::OnMassageKill, this, input);
            else _massageThread = thread(&Console::OnMassage, this, input);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////

    function<void(string)> onError = [&](string what)
    {
        ERROR_OUT(what);
    };

    function<void(tuple<int, int, float>)> onChangeProgress = [&](tuple<int, int, float> args)
    {
        auto& [step, allStep, progress] = args;
        if (allStep > 1) OUT("Progress(" << step << "/" << allStep << ") " << fixed << setprecision(1) << progress * 100 << " % ");
        else  OUT("Progress " << fixed << setprecision(1) << progress * 100 << " % ");
    };
    
    function<void(string)> onComplete = [&](string taskName)
    {
        OUT(taskName << " Completed");
    };
};