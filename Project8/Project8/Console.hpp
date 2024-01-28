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

    Event<void, tuple<string, bool>> _onLoadEvent;
    Event<void, bool> _onConvertEvent;
    Event<void, tuple<int, string>> _onPreviewEvent;
    Event<void, void> _onKillEvent;

    optional<thread> _massageThread = nullopt;
    optional<thread> _massageKillThread = nullopt;
    optional<cxxopts::Options> _options = nullopt;

    struct CmdAndExp
    {
        const string Cmd;
        const string Exp;
    };

    const CmdAndExp LoadMultiTheread   { "lmt", "load multi thread" };
    const CmdAndExp LoadPath           { "lpt", "load path" };
    const CmdAndExp ConvertMultiThread { "cmt", "convert multi thread" };
    const CmdAndExp PreviewSourceIndex { "psi", "preview source index" };
    const CmdAndExp PreviewEndPassName { "pep", "preview end pass name" };

public:
    IReadOnlyEvent<void, tuple<string, bool>>& OnLoadEvent = _onLoadEvent;
    IReadOnlyEvent<void, bool>& OnConvertEvent = _onConvertEvent;
    IReadOnlyEvent<void, tuple<int, string>>& OnPreviewEvent = _onPreviewEvent;
    IReadOnlyEvent<void, void>& OnKillEvent = _onKillEvent;

private:
    cxxopts::Options GetOptions(char* argv0)
    {
        using namespace cxxopts;

        Options options(argv0, "Star Composition System");

        options
            .custom_help("[Enter command after booting...]");

        options
            .allow_unrecognised_options()
            .add_options()
            ("h,help",              "show help")
            ("k",                   "kill command")
            ("l",                   "load imgs")
            (LoadMultiTheread.Cmd,   LoadMultiTheread.Exp,   cxxopts::value<bool>()->default_value("true"))
            (LoadPath.Cmd,           LoadPath.Exp,           cxxopts::value<string>()->default_value(""))
            ("c",                   "convert imgs")
            (ConvertMultiThread.Cmd, ConvertMultiThread.Exp, cxxopts::value<bool>()->default_value("true"))
            ("p",                   "preview img")
            (PreviewSourceIndex.Cmd, PreviewSourceIndex.Exp, cxxopts::value<int>()->default_value("0"))
            (PreviewEndPassName.Cmd, PreviewEndPassName.Exp, cxxopts::value<string>()->default_value(""))
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
                OnError("OnMassageIsNotRunning");
                match = true;
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////

            if (result.count(LoadMultiTheread.Cmd))
            {
                OnChangeParams({ LoadMultiTheread.Exp, result[LoadMultiTheread.Cmd].as<bool>() ? "true" : "false" });
                match = true;
            }
            if (result.count(LoadPath.Cmd))
            {
                OnChangeParams({ LoadPath.Exp , result[LoadPath.Cmd].as<string>() });
                match = true;
            }
            if (result.count("l"))
            {
                _onLoadEvent({ result[LoadPath.Cmd].as<string>(), result[LoadMultiTheread.Cmd].as<bool>() });
                match = true;
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////

            if (result.count(ConvertMultiThread.Cmd))
            {
                OnChangeParams({ PreviewSourceIndex.Exp, result[ConvertMultiThread.Cmd].as<bool>() ? "true" : "false" });
                match = true;
            }
            if (result.count("c"))
            {
                _onConvertEvent(result[ConvertMultiThread.Cmd].as<bool>());
                match = true;
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////

            if (result.count(PreviewSourceIndex.Cmd))
            {
                OnChangeParams({ PreviewSourceIndex.Exp, to_string(result[PreviewSourceIndex.Cmd].as<int>()) });
                match = true;
            }
            if (result.count(PreviewEndPassName.Cmd))
            {
                OnChangeParams({ PreviewEndPassName.Exp, result[PreviewEndPassName.Cmd].as<string>() });
                match = true;
            }
            if (result.count("p"))
            {
                _onPreviewEvent({ result[PreviewSourceIndex.Cmd].as<int>(), result[PreviewEndPassName.Cmd].as<string>() });
                match = true;
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////

            if (!match)
            {
                OnError("NotMuch");
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////
        }
        catch (OptionException e) 
        {
            OnError(e.what());
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
                _onKillEvent();
            }
            else
            {
                OnError("OnMassageIsRunning");
            }
        }
        catch (OptionException e)
        {
            OnError(e.what());
        }

        _massageKillThread.value().detach();
        _massageKillThread = nullopt;
    }

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

    void OnError(string what)
    {
        ERROR_OUT(what);
    }

    void OnChangeProgress(tuple<int, int, float> args)
    {
        auto& [step, allStep, progress] = args;
        if (allStep > 1) OUT("Progress(" << step << "/" << allStep << ") " << fixed << setprecision(1) << progress * 100 << " % ");
        else  OUT("Progress " << fixed << setprecision(1) << progress * 100 << " % ");
    }
    
    void OnComplete(string taskName)
    {
        OUT(taskName << " Completed");
    }
};