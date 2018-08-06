#ifndef TEST_TCP_ASYNC_CLIENT_H
#define TEST_TCP_ASYNC_CLIENT_H
#include "../comm.h"
using namespace std;
USING_NAMESPACE_S





// callback client --------------------------------------------------------------------------------------------------
class __TestTcpAsyncClientogic : public IConsoleAppLogic
{
private:
	enum __ETestCase
	{
		__ETestCase_none,
		__ETestCase_upload,
		__ETestCase_download,
		__ETestCase_c2sReq_s2cResp,
	};

#define __TestCallbackClientSpeedLogic_uploadPackSize (100 * 1024)
#define __TestCallbackClientSpeedLogic_uploadPackCount 4000
#define __TestCallbackClientSpeedLogic_downloadPackSize (10 * 1024)
#define __TestCallbackClientSpeedLogic_downloadPackCount 1000
#define __TestCallbackClientSpeedLogic_c2sReq_s2cResp_packSize (10 * 1024)
#define __TestCallbackClientSpeedLogic_c2sReq_s2cResp_packCount 1000
#define __TestCallbackClientSpeedLogic_paraPackCount 10




	virtual void onAppStartMsg(IConsoleAppApi * api) override
	{
		printf("client:: onAppStartMsg\n");

		m_uploaded_pack_count = 0;
		m_upload_pack_count = 0;
		byte_t* d = new byte_t[__TestCallbackClientSpeedLogic_uploadPackSize];
		m_upload_pack_data.attach(d, __TestCallbackClientSpeedLogic_uploadPackSize);


		m_console_api = api;
		initSocketLib();
		m_sapi = new SocketApi();
		if (!m_sapi->init(&(m_console_api->getMessageLooper())))
		{
			printf("client:: fail to m_sapi->init\n");
			m_console_api->exit();
			return;
		}

		m_sid = 0;
		ITcpAsyncClientApi::TcpClientSocketCreateParam param;
		param.m_notify_looper = &m_console_api->getMessageLooper();
		param.m_notify_target = this;
		param.m_svr_ip = SocketUtil::strToIp("127.0.0.1");
		param.m_svr_port = 12306;
		if (!m_sapi->atcp_createClientSocket(&m_sid, param))
		{
			printf("client:: fail to m_sapi->createClientSocket\n");
			m_console_api->exit();
			return;
		}

		if (!m_sapi->atcp_startClientSocket(m_sid, &m_connection_id))
		{
			printf("client:: fail to m_sapi->startClientSocket\n");
			m_console_api->exit();
			return;
		}

		m_testCase = __ETestCase_none;
	}

	virtual void onAppStopMsg() override
	{
		printf("client:: exiting...\n");
		delete m_sapi;
		releaseSocketLib();
		printf("client:: exited...\n");
	}

	virtual void onTextMsg(const std::string & textMsg) override
	{
		//if (!m_sapi->sendDataFromClientSocketToSvr(m_sid, (const byte_t*)textMsg.c_str(), textMsg.size() + 1))
		//{
		//	printf("client:: fail to sendDataFromClientSocketToSvr\n");
		//}
	}

	virtual void onMessage(Message * msg, bool* is_handled) override
	{
		if (msg->m_target != this)
			return;

		if (msg->m_sender == m_sapi)
		{
			switch (msg->m_msg_type)
			{
			case ITcpAsyncApi::EMsgType_tcpClientSocketConnected:
				__onMsg_clientConnected(msg);
				break;

			case ITcpAsyncApi::EMsgType_tcpClientSocketDisconnected:
				printf("client:: disconnected\n");
				break;

			case ITcpAsyncApi::EMsgType_tcpClientSocketRecvData:
				__onMsg_recvData(msg);
				break;

			case ITcpAsyncApi::EMsgType_tcpClientSocketSendDataEnd:
				__onMsg_sendDataEnd(msg);
				break;
			default:
				break;
			}
		}
	}

	void __onMsg_clientConnected(Message* msg)
	{
		printf("client:: connected\n");
		if (m_testCase != __ETestCase_none)
			return;

		m_testCase = __ETestCase_upload;
		//printf("client:: test upload\n");
		std::string str = std::string() + "test_case=upload_start"
			+ ",pack_size=" + StringUtil::toString(__TestCallbackClientSpeedLogic_uploadPackSize)
			+ ",pack_count=" + StringUtil::toString(__TestCallbackClientSpeedLogic_uploadPackCount)
			+ ",";
		m_upload_start_time_ms = TimeUtil::getMsTime();
		__sendPackToSvr(str);

		printf("client:: %s\n", str.c_str());
	}
	
	void __onMsg_recvData(Message* msg)
	{
		ITcpAsyncClientApi::Msg_TcpClientSocketRecvData* m = (ITcpAsyncClientApi::Msg_TcpClientSocketRecvData*)msg;
		if (m->m_recv_data.getLen() > 10)
		{
			const char* sz = (const char*)m->m_recv_data.getData();
			if (sz[0] == 't' && sz[1] == 'e' && strncmp(sz, "test_case=", strlen("test_case=")) == 0)
			{
				std::string recv_str;
				recv_str.append((const char*)m->m_recv_data.getData());
				printf("client:: recv=%s\n", recv_str.c_str());

				std::string test_case_str = StringUtil::fetchMiddle(recv_str, "test_case=", ",");
				if (test_case_str == "download_end")
				{

				}
			}
		}
	}

	void __onMsg_sendDataEnd(Message* msg)
	{
		//printf("client:: send end\n");

		__onSentPack();

		if (m_testCase == __ETestCase_upload)
		{
			if (m_upload_pack_count == 0)
			{
				for (int i = 0; i < __TestCallbackClientSpeedLogic_paraPackCount; ++i)
				{
					__sendPackToSvr(m_upload_pack_data.getData(), m_upload_pack_data.getLen());
				}
			}
			else
			{
				++m_uploaded_pack_count;
				//printf("send pack end, count=%llu\n", m_uploaded_pack_count);

				if (m_upload_pack_count < __TestCallbackClientSpeedLogic_uploadPackCount)
				{
					__sendPackToSvr(m_upload_pack_data.getData(), m_upload_pack_data.getLen());
				}

				if (m_uploaded_pack_count == __TestCallbackClientSpeedLogic_uploadPackCount)
				{
					std::string str = std::string() + "test_case=upload_end";
					__sendPackToSvr(str);

					m_testCase = __ETestCase_download;
				}
			}
		}
		else if (m_testCase == __ETestCase_download)
		{
			uint64_t total_size = m_send_total_len;
			uint64_t total_time_span_ms = TimeUtil::getMsTime() - m_upload_start_time_ms;
			uint64_t byte_per_second = total_size * 1000 / total_time_span_ms;
			printf("\nclient:: upload end, total_size=%s, sent_ms=%" PRIu64 ", byte_per_second=%s\n"
				, StringUtil::byteCountToDisplayString(total_size).c_str(), total_time_span_ms, StringUtil::byteCountToDisplayString(byte_per_second).c_str());

			//str = std::string() + "test_case=download_start"
			//	+ ",pack_size=" + StringUtil::toString(__TestCallbackClientSpeedLogic_downloadPackSize)
			//	+ ",pack_count=" + StringUtil::toString(__TestCallbackClientSpeedLogic_downloadPackCount)
			//	+ ",";
			//__sendPackToSvr(str);
			printf("client:: close socket\n");
			m_sapi->atcp_releaseClientSocket(m_sid);
			m_sid = 0;
		}
	}

	void __onSentPack()
	{
		const uint64_t block_size = 1 * 1024 * 1024;
		uint64_t send_block_count = m_upload_pack_count * __TestCallbackClientSpeedLogic_uploadPackSize / block_size;

		while (send_block_count > m_already_print_send_block_count)
		{
			++m_already_print_send_block_count;
			printf(".");
		}
	}

	bool __sendPackToSvr(const std::string& str)
	{
		if (!m_sapi->atcp_sendDataFromClientSocketToSvr(m_sid, m_connection_id, (const byte_t*)str.c_str(), str.size() + 1))
		{
			printf("fail to sendDataFromClientSocketToSvr\n");
			return false;
		}
		m_send_total_len += str.size() + 1;
		return true;
	}

	bool __sendPackToSvr(const byte_t* data, size_t data_len)
	{
		bool is_ok = m_sapi->atcp_sendDataFromClientSocketToSvr(m_sid, m_connection_id, data, data_len);
		if (!is_ok)
		{
			printf("fail to __sendPackToSvr\n");
			return false;
		}
		m_upload_pack_count++;
		//if(m_upload_pack_count == __TestCallbackClientSpeedLogic_uploadPackCount)
		//	printf("upload all data to svr ok, m_upload_pack_count=%u\n", (uint32_t)m_upload_pack_count);


		m_send_total_len += __TestCallbackClientSpeedLogic_uploadPackSize;
		return is_ok;
	}





	IConsoleAppApi* m_console_api;
	SocketApi* m_sapi;
	socket_id_t m_sid;
	uint64_t m_connection_id;
	__ETestCase m_testCase;
	size_t m_uploaded_pack_count;
	size_t m_upload_pack_count;
	Binary m_upload_pack_data;
	uint64_t m_upload_start_time_ms;
	uint64_t m_send_total_len = 0;
	uint64_t m_already_print_send_block_count = 0;
};







void __testTcpAsyncClient()
{
	printf("\n__testTcpAsyncClient ---------------------------------------------------------\n");
	__initLog(ELogLevel_debug);
	ConsoleApp* app = new ConsoleApp();
	__TestTcpAsyncClientogic* logic = new __TestTcpAsyncClientogic();
	app->run(logic);
	delete logic;
	delete app;
}




#endif

