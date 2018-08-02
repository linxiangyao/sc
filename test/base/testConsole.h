#ifndef __TEST_CONSOLE
#define __TEST_CONSOLE

#include "../../src/console/consoleApp.h"
using namespace std;
USING_NAMESPACE_S




class __ConsoleAppLogic : public IConsoleAppLogic
{
private:
    virtual void onAppStartMsg(IConsoleAppApi* api) override
    {
        printf("logic:: onAppStartMsg\n");
        m_api = api;
    }
	
    virtual void onAppStopMsg() override
    {
        printf("logic:: onAppStopMsg\n");
    }
    
    virtual void onTextMsg(const std::string& msg) override
    {
        printf("logic:: onTextMsg, text=%s\n", msg.c_str());
    }
    
    virtual void onMessage(Message* msg, bool* isHandled) override
    {
        
    }

	virtual void onMessageTimerTick(uint64_t timer_id, void* user_data) override
	{

	}
    
    IConsoleAppApi* m_api;
};

void __testConsoleApp()
{
	printf("\n__testConsoleApp ---------------------------------------------------------\n");
    __ConsoleAppLogic* logic = new __ConsoleAppLogic();
    ConsoleApp* app = new ConsoleApp();
    app->run(logic);
    delete logic;
    delete app;
}


void __testConsole()
{
	__testConsoleApp();
}


#endif // !__TEST_MISC
