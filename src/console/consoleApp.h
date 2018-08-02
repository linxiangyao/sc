#ifndef S_CONSOLE_APP_H_
#define S_CONSOLE_APP_H_
#include <vector>
#include <stdio.h>
#include "../comm/comm.h"
#include "../thread/threadLib.h"
#include "../util/consoleUtil.h"
S_NAMESPACE_BEGIN


class IConsoleAppApi
{
public:
    virtual ~IConsoleAppApi() {}
	virtual MessageLooper& getMessageLooper() = 0;
    virtual void exit() = 0;
};


// if user enter exit, console will quit.
class IConsoleAppLogic : public IMessageLoopHandler
{
public:
    virtual void onAppStartMsg(IConsoleAppApi* api) = 0;
    virtual void onAppStopMsg() = 0;
	virtual void onTextMsg(const std::string& text_msg) {}
	virtual void onMessage(Message* msg, bool* is_handled) {}
	virtual void onMessageTimerTick(uint64_t timer_id, void* user_data) {}
};


class ConsoleApp : public IMessageLoopHandler, public IConsoleAppApi
{
public:
    ConsoleApp();
    ~ConsoleApp();

    void run(IConsoleAppLogic* logic);
	virtual void exit();
	virtual MessageLooper& getMessageLooper();


private:
    class __InputRun : public IThreadRun
    {
    public:
		__InputRun(IConsoleAppApi* api) { m_api = api; m_is_exit = false; }

    private:
        virtual void run();
        virtual void stop();
        IConsoleAppApi* m_api;
		bool m_is_exit;
    };

    
	virtual void onMessage(Message* msg, bool* is_handled);
	virtual void onMessageTimerTick(uint64_t timer_id, void* user_data);

    
    bool m_is_exit;
    Mutex m_mutex;
    IConsoleAppLogic* m_logic;
    MessageLooper* m_looper;
    Thread* m_input_thread;
	__InputRun* m_input_run;
};


class ConsoleArgs
{
public:
	ConsoleArgs()
	{
	}

	ConsoleArgs(int argc, char** argv)
	{
		parseArgs(argc, argv);
	}

	void parseArgs(int argc, char** argv)
	{
		m_args.clear();
		for (int i = 0; i < argc; ++i)
		{
			std::string str = argv[i];
			std::string key = StringUtil::trim(StringUtil::fetchMiddle(argv[i], "--", "="));
			if (key.size() == 0)
				continue;
			size_t index = str.find("=");
			if (index == std::string::npos || index == str.size() - 1)
				continue;

			std::string value = StringUtil::trim(str.substr(index + 1));
			m_args[key] = value;
		}
	}

	std::map<std::string, std::string> m_args;
};



S_NAMESPACE_END
#endif
