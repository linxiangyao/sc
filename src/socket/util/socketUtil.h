#ifndef S_SOCKET_UTIL_H_
#define S_SOCKET_UTIL_H_
#include "../base/socketType.h"
S_NAMESPACE_BEGIN



class SocketUtil
{
public:
	static socket_id_t genSid();
	static uint64_t genConnectionId();
	static socket_t openSocket(ESocketType socket_type);
	static bool bindAndListen(socket_t s, const std::string& svr_ip, int svr_port);
	static bool bind(socket_t s, Ip ip, int svr_prot);
	static bool accept(socket_t svr_accept_socket, socket_t* svr_trans_socket);
	static bool connect(socket_t client_socket, const std::string& svr_ip_or_name, int svr_port);
	static bool connect(socket_t client_socket, Ip svr_ip, int svr_port);
	static void disconnect(socket_t client_socket);
	static bool send(socket_t s, const byte_t* data, size_t data_len);
	static bool send(socket_t s, const byte_t* data, size_t data_len, size_t* real_send_len);
	static bool recv(socket_t s, byte_t* buf, size_t buf_len, size_t* real_recv_len);
	static bool sendTo(socket_t s, const byte_t* data, size_t data_len, Ip to_ip, uint32_t to_port);
	static bool sendTo(socket_t s, const byte_t* data, size_t data_len, Ip to_ip, uint32_t to_port, size_t* real_send_len);
	static bool recvFrom(socket_t s, byte_t* buf, size_t buf_len, Ip* from_ip, uint32_t* from_port, size_t* real_recv_len);
	static void closeSocket(socket_t s);
	static bool changeSocketToAsync(socket_t s);
    static bool resolveIpsByName(const std::string& name, std::vector<Ip>* ips); // will block
	static Ip resolveIpByName(const std::string& name);
	static int getErr();

	static bool ipToStr(const sockaddr* addr, std::string* ip_str);
	static bool ipToStr(Ip ip, std::string* ip_str);
	static std::string ipToStr(Ip ip);
    static bool ipv4ToStr(in_addr ip_v4, std::string* ip_str);
	static bool ipv6ToStr(in6_addr ip_v6, std::string* ip_str);
	static bool strToIp(const std::string& ip_str, Ip* ip);
	static Ip strToIp(const std::string& ip_str);
    static bool strToIpV4(const std::string& ip_str, in_addr* ip_v4);
	static bool strToIpV6(const std::string& ip_str, in6_addr* ip_v6);
	static bool sockaddrToIpAndPort(const sockaddr* addr, uint32_t addr_len, Ip* ip, uint32_t* port);
	static bool ipAndPortToSockaddr(Ip ip, uint32_t port, sockaddr_storage* addr, uint32_t* addr_len);
    static uint16_t hToNs(uint16_t s);
    static uint32_t hToNl(uint32_t l);
    static uint16_t nToHs(uint16_t s);
    static uint32_t nToHl(uint32_t l);
    static bool isValidSid(socket_id_t s);




private:
	static void __initAddrV4(struct sockaddr_in* addr, in_addr ip, int port);
	static void __initAddrV6(struct sockaddr_in6* addr, in6_addr ip, int port);
	static void __initAddrStorage(struct sockaddr_storage* addr, int* addr_len, Ip ip, int port);
	static Ip __resovleIpByName(const std::string& ip_or_name);
	static bool __recvFrom(socket_t s, byte_t* buf, size_t buf_len, sockaddr_storage* addr, uint32_t* addr_len, size_t* real_recv_len);

	static Mutex s_sid_mutex;
	static socket_id_t m_sid_seed;
	static uint64_t m_connection_id_seed;
};





S_NAMESPACE_END
#endif


