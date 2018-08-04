#ifndef TESTSOCKET_H
#define TESTSOCKET_H
#include "../comm.h"
#include "../../src/socket/socketLib.h"
using namespace std;
USING_NAMESPACE_S


// ip --------------------------------------------------------------------------------------------------
void __testGetIpByName()
{
	//printf("\n__testGetIpByName ---------------------------------------------------------\n");
	//std::vector<uint32_t> ips;
 //   uint32_t ip = 0;
 //   std::string ipStr;


 //   if (SocketUtil::getIpByName("192.168.0.1", &ips))
 //   {
	//	ip = ips[0];
 //       SocketUtil::ipToStr(ip, &ipStr);
 //       __print("192.168.0.1 ip=%u, ipStr=%s\n", ip, ipStr.c_str());
 //   }
 //   else
 //   {
 //       __print("192.168.0.1 fail to getIpByName\n");
 //   }


 //   if (SocketUtil::getIpByName("www.baidu.com", &ips))
 //   {
	//	ip = ips[0];
 //       SocketUtil::ipToStr(ip, &ipStr);
 //       __print("www.baidu.com ip=%u, ipStr=%s\n", ip, ipStr.c_str());
 //   }
 //   else
 //   {
 //       __print("www.baidu.com fail to getIpByName\n");
 //   }


 //   if (SocketUtil::getIpByName("www.baiduxxdfewasdfwecawef.com", &ips))
 //   {
	//	ip = ips[0];
 //       SocketUtil::ipToStr(ip, &ipStr);
 //       __print("www.baiduxxdfewasdfwecawef.com ip=%u, ipStr=%s\n", ip, ipStr.c_str());
 //   }
 //   else
 //   {
 //       __print("www.baiduxxdfewasdfwecawef.com fail to getIpByName\n");
 //   }


 //   if (SocketUtil::getIpByName("http://www.baidu.com", &ips))
 //   {
	//	ip = ips[0];
 //       SocketUtil::ipToStr(ip, &ipStr);
 //       __print("http://www.baidu.com ip=%u, ipStr=%s\n", ip, ipStr.c_str());
 //   }
 //   else
 //   {
 //       __print("http://www.baidu.com fail to getIpByName\n");
 //   }


 //   if (SocketUtil::getIpByName("http://www.baidusafasdfwerwerwerwer.com", &ips))
 //   {
	//	ip = ips[0];
 //       SocketUtil::ipToStr(ip, &ipStr);
 //       __print("http://www.baidusafasdfwerwerwerwer.com ip=%u, ipStr=%s\n", ip, ipStr.c_str());
 //   }
 //   else
 //   {
 //       __print("http://www.baidusafasdfwerwerwerwer.com fail to getIpByName\n");
 //   }
}






// callback server --------------------------------------------------------------------------------------------------
void __testSocketUtil()
{
	__initLog(ELogLevel_debug);
	initSocketLib();
	//__testGetIpByName();
}



#endif
