#include <fstream>  
#include "consoleApp.h"
#include "../util/consoleUtil.h"
#include "../util/fileUtil.h"
S_NAMESPACE_BEGIN

#define __MSG_TYPE_ConsoleApp_recvText -811731
#define __MSG_TYPE_ConsoleApp_startCmd -811732
#define __MSG_TYPE_ConsoleApp_exitCmd -811733



ConsoleApp::ConsoleApp()
{
}

ConsoleApp::~ConsoleApp()
{
}

void ConsoleApp::run(IConsoleAppLogic* logic)
{
    printf("ConsoleApp ------------------\n");
	printf("ConsoleApp:: enter exit to quit\n");
    m_logic = logic;
    
    m_is_exit = false;

    m_input_run = new __InputRun(this);
    m_input_thread = new Thread(m_input_run);
    m_input_thread->start();
    
    m_looper = new MessageLooper();
    m_looper->addMsgHandler(this);
	m_looper->addMsgTimerHandler(this);
    Message* msg = new Message();
	msg->m_sender = this;
	msg->m_target = this;
    msg->m_msg_type = __MSG_TYPE_ConsoleApp_startCmd;
    m_looper->postMessage(msg);
    m_looper->loop();

    m_input_thread->stopAndJoin();

	m_logic->onAppStopMsg();
    delete m_input_thread;
    delete m_looper;

    printf("ConsoleApp:: exited ------------------\n");
}

void ConsoleApp::exit()
{
    ScopeMutex _l(m_mutex);
	if (m_is_exit)
		return;
    m_is_exit = true;
    Message* msg = new Message();
	msg->m_sender = this;
	msg->m_target = this;
    msg->m_msg_type = __MSG_TYPE_ConsoleApp_exitCmd;
    m_looper->postMessage(msg);
}

MessageLooper& ConsoleApp::getMessageLooper()
{
	return *m_looper;
}

void ConsoleApp::onMessage(Message* msg, bool* is_handled)
{
	// special msg: start
	if (msg->m_sender == this && msg->m_msg_type == __MSG_TYPE_ConsoleApp_startCmd)
	{
		m_logic->onAppStartMsg(this);
		return;
	}
	// special msg: exit
	else if ((msg->m_sender == this || msg->m_sender == m_input_run) && msg->m_msg_type == __MSG_TYPE_ConsoleApp_exitCmd)
	{
		m_is_exit = true;
		m_looper->stopLoop();
		m_input_thread->stop();
		return;
	}

    if (m_is_exit)
        return;

	// normal message
	if (msg->m_sender == m_input_run)
	{
		if(msg->m_msg_type == __MSG_TYPE_ConsoleApp_recvText)
			m_logic->onTextMsg(msg->m_args.getString("text"));
		return;
	}
	
	m_logic->onMessage(msg, is_handled);
}

void ConsoleApp::onMessageTimerTick(uint64_t timer_id, void* user_data)
{
	if (m_is_exit)
		return;
	m_logic->onMessageTimerTick(timer_id, user_data);
}





void ConsoleApp::__InputRun::run()
{
	sscope_d();
    while(true)
    {
        std::string text;
        ConsoleUtil::pauseConsoleAndGetInput(&text);
		if (m_is_exit)
			return;
        
        Message* msg = new Message();
		msg->m_sender = this;

        if(text == "exit\n")
        {
			printf("\nConsoleApp:: start to exit\n");
			m_api->exit();
            break;
        }
        
        msg->m_msg_type = __MSG_TYPE_ConsoleApp_recvText;
        msg->m_args.setString("text", text);
        m_api->getMessageLooper().postMessage(msg);
    }
}

void ConsoleApp::__InputRun::stop()
{
	m_is_exit = true;
}


S_NAMESPACE_END







//FileUtil::writeFile("input.txt", "exit\n");
//freopen("input.txt", "r", stdin);
//FileUtil::writeFile("input.txt", "exit\n");

//FileUtil::writeFile("input.txt", "exit\n");
//std::ifstream fin("input.txt");
//std::cin.rdbuf(fin.rdbuf());
//FileUtil::writeFile("input.txt", "exit\n");
//fclose(stdin);

