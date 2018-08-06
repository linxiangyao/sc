#ifndef S_SOCKET_EPOLLUTIL_H_
#define S_SOCKET_EPOLLUTIL_H_
#include "socketSelector.h"
S_NAMESPACE_BEGIN
#define S_OS_LINUX
#if  defined(S_OS_LINUX) || defined(S_OS_ANDROID)
#include <sys/epoll.h>



class EpollUtil
{
public:
#define __EPOLL_UTIL_RECV_BUF (10 * 1024)
    class EpollRun : public IThreadRun
    {
    public:
		EpollRun(const MessageLooperNotifyParam& notify_param)
        {
			slog_d("EpollRun this=%0", (uint64_t)this);
			m_notify_param = notify_param;
            m_is_exit = false;
            m_is_wakeuping = false;
            m_pipe[0] = -1;
            m_pipe[1] = -1;
            
            if (pipe(m_pipe) != 0)
            {
                slog_e("EpollRun:: fail to pipe");
                return;
            }
			m_pipe_selector_session_id = SocketSelector::genSessionId();

            m_epoll_fd = epoll_create(100 * 1000);
            if(m_epoll_fd == -1)
            {
                slog_e("EpollRun:: fail to epoll_create");
                return;
            }

			if (!__addFdEvent(m_pipe[0], EPOLLIN, m_pipe_selector_session_id))
			{
				slog_e("EpollRun:: fail to __addFdEvent");
				return;
			}
        }

		~EpollRun()
		{
			slog_d("~EpollRun this=%0", (uint64_t)this);
		}
        
        virtual void run() override 
        {
            slog_d("EpollRun:: run");
            __onRun();
            
            ScopeMutex __l(m_mutex);
            __releaseAll();
            slog_d("EpollRun:: exit");
        }
        
		virtual void stop() override 
        {
            ScopeMutex __l(m_mutex);
            if(m_is_exit)
                return;
            slog_d("EpollRun:: stop this=%0", (uint64_t)this);
            __wakeup();
            m_is_exit = true;
        }
        
        bool addAcceptSocket(socket_t socket, uint64_t session_id)
        {
            ScopeMutex __l(m_mutex);
            if(m_is_exit)
                return false;
			
            if(is_map_contain_key(m_ctxs, session_id))
                return false;
            __SocketCtx* ctx = new __SocketCtx(socket, __ESocketType_tcp_accept, session_id);
            m_ctxs[session_id] = ctx;
            if(!__addFdEvent(socket, EPOLLIN, session_id))
			{
				slog_e("EpollRun:: addAcceptSocket fail to __addFdEvent");
				return false;
			}
			
            __wakeup();
            return true;
        }

        bool addTranSocket(socket_t socket, uint64_t session_id, bool is_tcp)
        {
            ScopeMutex __l(m_mutex);
            if(m_is_exit)
                return false;
			
            if(is_map_contain_key(m_ctxs, session_id))
                return false;
            __SocketCtx* ctx = new __SocketCtx(socket, is_tcp ? __ESocketType_tcp_tran : __ESocketType_udp, session_id);
            m_ctxs[session_id] = ctx;
            if(!__addFdEvent(socket, EPOLLIN, session_id))
			{
				slog_e("EpollRun:: addTranSocket fail to __addFdEvent");
				return false;
			}
            
			slog_i("EpollRun:: add tran, is_tcp=%0, ctx->socket_type=%1", is_tcp, ctx->m_socket_type);
            __wakeup();
            return true;
        }

        bool postSendCmd(socket_t socket, uint64_t session_id, uint64_t send_cmd_id, const byte_t* data, size_t data_len)
        {
            ScopeMutex __l(m_mutex);
            if(m_is_exit)
                return false;
			
            __SocketCtx* ctx = get_map_element_by_key(m_ctxs, session_id);
            if(ctx == nullptr)
                return false;
            __SendCmd* cmd = new __SendCmd(send_cmd_id, data, data_len);
            ctx->m_send_cmds.push_back(cmd);
			if(ctx->m_send_cmds.size() == 1)
			{
				if(!__modFdEvent(socket, EPOLLIN | EPOLLOUT, ctx->m_session_id))
				{
					slog_e("EpollRun:: postSendCmd fail to __modFdEvent");
					return false;
				}
			}
           
		   slog_d("EpollRun:: post send");
            __wakeup();
            return true;
        }

	    bool postSendToCmd(socket_t socket, uint64_t session_id, uint64_t send_cmd_id, Ip to_ip, uint32_t to_port, const byte_t* data, size_t data_len)
        {
            ScopeMutex __l(m_mutex);
            if(m_is_exit)
                return false;
			
            __SocketCtx* ctx = get_map_element_by_key(m_ctxs, session_id);
            if(ctx == nullptr)
                return false;
            __SendToCmd* cmd = new __SendToCmd(send_cmd_id, data, data_len, to_ip, to_port);
            ctx->m_send_to_cmds.push_back(cmd);
			if(ctx->m_send_to_cmds.size() == 1)
			{
				if(!__modFdEvent(socket, EPOLLIN | EPOLLOUT, ctx->m_session_id))
				{
					slog_e("EpollRun:: postSendToCmd fail to __modFdEvent");
					return false;
				}
			}
			
			//slog_v("EpollRun:: post sendto");
            __wakeup();
            return true;
        }
        
		void removeSocket(socket_t socket, uint64_t session_id)
		{
			ScopeMutex __l(m_mutex);
			if (m_is_exit)
				return;

			__SocketCtx* ctx = get_map_element_by_key(m_ctxs, session_id);
			if (ctx == nullptr)
				return;
			slog_d("EpollRun:: removeSocket, session_id=%0", session_id);
			__releaseCtx(ctx);
		}
		




    private:
		enum __ESocketType
		{
			__ESocketType_tcp_accept,
			__ESocketType_tcp_tran,
			__ESocketType_udp,
		};

        class __SendCmd
        {
        public:
            __SendCmd(uint64_t send_cmd_id, const byte_t* data, size_t data_len) { m_send_cmd_id = send_cmd_id; m_send_data.copy(data, data_len); }
            uint64_t m_send_cmd_id;
            Binary m_send_data;
        };
        
        class __SendToCmd
        {
        public:
            __SendToCmd(uint64_t send_cmd_id, const byte_t* data, size_t data_len, Ip to_ip, uint32_t to_port)  
            { m_send_cmd_id = send_cmd_id; m_send_data.copy(data, data_len); m_to_ip = to_ip; m_to_port = to_port; }

            uint64_t m_send_cmd_id;
            Binary m_send_data;
            Ip m_to_ip;
            uint32_t m_to_port;
        };
        
        class __SocketCtx
        {
        public:
            __SocketCtx() { m_socket = INVALID_SOCKET; m_session_id = 0; }
            __SocketCtx(socket_t s, __ESocketType socket_type, uint64_t session_id) { m_socket = s; m_socket_type = socket_type; m_session_id = session_id; }
            socket_t m_socket;
			__ESocketType m_socket_type;
            uint64_t m_session_id;
            std::vector<__SendCmd*> m_send_cmds;
            std::vector<__SendToCmd*> m_send_to_cmds;
        };
        
        void __onRun()
        {   
            static const int MAX_EVENTS = 10;
            epoll_event evs[MAX_EVENTS];
            while(!m_is_exit)
            {
                int event_count = epoll_wait(m_epoll_fd, evs, MAX_EVENTS, -1);

                ScopeMutex __l(m_mutex);
				if(m_is_exit)
					return;

                if (event_count == -1)
                {
                    slog_e("EpollRun:: onRun fail to epoll_wait");
                    return;
                }

                m_is_wakeuping = false;
				
                for (int i = 0 ; i < event_count; i++) 
                {
					epoll_event& ev = evs[i];
					
					if(ev.data.u64 == m_pipe_selector_session_id)
						__onWakeupEvent();
					else
						__onSocketEvent(&ev);
                }
            }
        }

		void __onWakeupEvent()
		{
			char b;
			::read(m_pipe[0], &b, 1);
			//slog_v("EpollRun:: onRun recv signal");
		}
		
		void __onSocketEvent(epoll_event* ev)
		{
			__SocketCtx* ctx = get_map_element_by_key(m_ctxs, ev->data.u64);
			if(ctx == nullptr)
			{
				__delFdEvent(ev->data.fd);
				return;
			}

			if(ctx->m_socket_type == __ESocketType_tcp_accept)
			{
				__onAcceptSocketEvent(ctx, ev);
			}
			else if (ctx->m_socket_type == __ESocketType_tcp_tran)
			{
				__onTcpTranSocketEvent(ctx, ev);
			}
			else if (ctx->m_socket_type == __ESocketType_udp)
			{
				__onUdpSocketEvent(ctx, ev);
			}
			else
			{
				slog_d("EpollRun:: unknown socket_type=%0", ctx->m_socket_type);
			}
		}

		void __onAcceptSocketEvent(__SocketCtx* ctx, epoll_event* ev)
		{
			// err: release resource and post msg
			if (ev->events & EPOLLERR || ev->events & EPOLLHUP)
			{
				slog_d("EpollRun:: accept closed");
				__postMsg_SocketClosed_and_releaseCtx(ctx);
				return;
			}

			// ok: accept and post msg
			if (ev->events & EPOLLIN)
			{
				socket_t tran_socket = INVALID_SOCKET;
				if (!SocketUtil::accept(ctx->m_socket, &tran_socket))
				{
					slog_e("EpollRun:: fail to SocketUtil::accept");
					__postMsg_SocketClosed_and_releaseCtx(ctx);
					return;
				}

				__postMsg_AcceptOk(ctx, tran_socket);
				slog_d("EpollRun:: accept one client, tran_socket=%0", tran_socket);
			}
		}

		void __onTcpTranSocketEvent(__SocketCtx* ctx, epoll_event* ev)
		{
			if (ev->events & EPOLLERR || ev->events & EPOLLHUP)
			{
				slog_d("EpollRun:: tcp tran closed");
				__postMsg_SocketClosed_and_releaseCtx(ctx);
				return;
			}

			// recv ready
			if (ev->events & EPOLLIN)
			{
				size_t real_recv_len = 0;
				if (!SocketUtil::recv(ctx->m_socket, m_recv_buf, __EPOLL_UTIL_RECV_BUF, &real_recv_len))
				{
					slog_e("EpollRun:: fail to SocketUtil::recv");
					__postMsg_SocketClosed_and_releaseCtx(ctx);
					return;
				}

				__postMsg_RecvOk(ctx, m_recv_buf, real_recv_len);
			}

			// send ready
			if (ev->events & EPOLLOUT)
			{
				// has data to send
				if (ctx->m_send_cmds.size() > 0)
				{
					__SendCmd* cmd = ctx->m_send_cmds[ctx->m_send_cmds.size() - 1];
					size_t real_send_len = 0;
					if (!SocketUtil::send(ctx->m_socket, cmd->m_send_data.getData(), cmd->m_send_data.getLen(), &real_send_len))
					{
						slog_e("EpollRun:: fail to SocketUtil::send");
						__postMsg_SocketClosed_and_releaseCtx(ctx);
						return;
					}

					if (real_send_len = cmd->m_send_data.getLen())
					{
						__postMsg_SendEnd(ctx, cmd, true);
						delete_and_erase_vector_element_by_index(&ctx->m_send_cmds, ctx->m_send_cmds.size() - 1);
					}
					else
					{
						cmd->m_send_data.shrinkBegin(real_send_len);
					}
				}
				// no data to send
				else
				{
					__modFdEvent(ctx->m_socket, EPOLLIN, ctx->m_session_id);
				}
			}
		}

		void __onUdpSocketEvent(__SocketCtx* ctx, epoll_event* ev)
		{
			if (ev->events & EPOLLERR || ev->events & EPOLLHUP)
			{
				slog_d("EpollRun:: udp closed");
				__postMsg_SocketClosed_and_releaseCtx(ctx);
				return;
			}

			// recv ready
			if (ev->events & EPOLLIN)
			{
				Ip from_ip;
				uint32_t from_port = 0;
				size_t real_recv_len = 0;
				if (!SocketUtil::recvFrom(ctx->m_socket, m_recv_buf, __EPOLL_UTIL_RECV_BUF, &from_ip, &from_port, &real_recv_len))
				{
					slog_e("EpollRun:: fail to SocketUtil::recvFrom");
					__postMsg_SocketClosed_and_releaseCtx(ctx);
					return;
				}

				__postMsg_RecvFromOk(ctx, from_ip, from_port, m_recv_buf, real_recv_len);
			}

			// send ready
			if (ev->events & EPOLLOUT)
			{
				// has data to send
				if (ctx->m_send_to_cmds.size() > 0)
				{
					__SendToCmd* cmd = ctx->m_send_to_cmds[ctx->m_send_to_cmds.size() - 1];
					size_t real_send_len = 0;
					if (!SocketUtil::sendTo(ctx->m_socket, cmd->m_send_data.getData(), cmd->m_send_data.getLen(), cmd->m_to_ip, cmd->m_to_port, &real_send_len))
					{
						slog_e("EpollRun:: fail to SocketUtil::sendTo");
						__postMsg_SocketClosed_and_releaseCtx(ctx);
						return;
					}

					if (real_send_len = cmd->m_send_data.getLen())
					{
						__postMsg_SendToEnd(ctx, cmd, true);
						delete_and_erase_vector_element_by_index(&ctx->m_send_to_cmds, ctx->m_send_to_cmds.size() - 1);
					}
					else
					{
						cmd->m_send_data.shrinkBegin(real_send_len);
					}
				}
				// no data to send
				else
				{
					__modFdEvent(ctx->m_socket, EPOLLIN, ctx->m_session_id);
				}
			}
		}
        
        void __wakeup()
        {
			if (m_is_exit)
				return;
            if(m_is_wakeuping)
                return;
            m_is_wakeuping = true;
            //slog_i("EpollRun:: __wakeup");
            write(m_pipe[1], "0", 1);		
        }
        
        void __releaseAll()
        {
		    if (m_pipe[0] != -1)
		    {
                close(m_pipe[0]);
                close(m_pipe[1]);
                m_pipe[0] = -1;
                m_pipe[1] = -1;
            }
            if(m_epoll_fd != -1)
            {
                close(m_epoll_fd);
                m_epoll_fd = -1;
            }
			for (auto it = m_ctxs.begin(); it != m_ctxs.end(); ++it)
			{
				SocketUtil::closeSocket(it->second->m_socket);
			}
			delete_and_erase_collection_elements(&m_ctxs);
        }
		
		bool __addFdEvent(int fd, int events, uint64_t selector_session_id)
		{
            epoll_event ev;
            ev.data.u64 = selector_session_id;
            ev.events = events;
			if(epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1)
			{
                slog_e("EpollRun:: __addEventToFd fail to epoll_ctl");
				return false;
			}
			return true;
		}
		
		bool __delFdEvent(int fd)
		{
            epoll_event ev;
			return epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, &ev) != -1;
		}
		
		bool __modFdEvent(int fd, int events, uint64_t selector_session_id)
		{
            epoll_event ev;
            ev.data.u64 = selector_session_id;
            ev.events = events;
			return epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, fd, &ev) != -1;
		}
        
		void __postMsg_SocketClosed(__SocketCtx* ctx)
        {
			SocketSelector::Msg_socketClosed * msg = new SocketSelector::Msg_socketClosed();
			__copyCtxToMsg(ctx, msg);
			m_notify_param.postMessage(msg);
        }

		void __postMsg_AcceptOk(__SocketCtx* ctx, socket_t tran_socket)
		{
			SocketSelector::Msg_acceptOk* msg = new SocketSelector::Msg_acceptOk();
			__copyCtxToMsg(ctx, msg);
			msg->m_tran_socket = tran_socket;
			m_notify_param.postMessage(msg);
		}

		void __postMsg_SendToEnd(__SocketCtx* ctx, __SendToCmd* cmd, bool is_ok)
		{
			SocketSelector::Msg_sendToEnd* msg = new SocketSelector::Msg_sendToEnd();
			__copyCtxToMsg(ctx, msg);
			msg->m_cmd_id = cmd->m_send_cmd_id;
			msg->m_is_ok = is_ok;
			m_notify_param.postMessage(msg);
		}

		void __postMsg_RecvFromOk(__SocketCtx* ctx, Ip from_ip, uint32_t from_port, const byte_t* data, size_t data_len)
		{
			SocketSelector::Msg_RecvFromOk* msg = new SocketSelector::Msg_RecvFromOk();
			__copyCtxToMsg(ctx, msg);
			msg->m_from_ip = from_ip;
			msg->m_from_port = from_port;
			msg->m_recv_data.append(data, data_len);
			m_notify_param.postMessage(msg);
		}

		void __postMsg_RecvOk(__SocketCtx* ctx, const byte_t* data, size_t data_len)
		{
			SocketSelector::Msg_RecvOk* msg = new SocketSelector::Msg_RecvOk();
			__copyCtxToMsg(ctx, msg);
			msg->m_recv_data.append(data, data_len);
			m_notify_param.postMessage(msg);
		}

		void __postMsg_SendEnd(__SocketCtx* ctx, __SendCmd* cmd, bool is_ok)
		{
			SocketSelector::Msg_sendEnd* msg = new SocketSelector::Msg_sendEnd();
			__copyCtxToMsg(ctx, msg);
			msg->m_cmd_id = cmd->m_send_cmd_id;
			msg->m_is_ok = is_ok;
			m_notify_param.postMessage(msg);
		}

		void __postMsg_SocketClosed_and_releaseCtx(__SocketCtx* ctx)
		{
			__postMsg_SocketClosed(ctx);
			__releaseCtx(ctx);
		}

		void __releaseCtx(__SocketCtx* ctx)
		{
			if (ctx == nullptr)
				return;
			__delFdEvent(ctx->m_socket);
			SocketUtil::closeSocket(ctx->m_socket);
			delete_and_erase_map_element_by_key(&m_ctxs, ctx->m_session_id);
		}

		void __copyCtxToMsg(__SocketCtx* ctx, SocketSelector::SocketSelector::BaseMsg* msg)
		{
			msg->m_socket = ctx->m_socket;
			msg->m_session_id = ctx->m_session_id;
		}
		




        bool m_is_exit;
        Mutex m_mutex;
	    int m_pipe[2];
		uint64_t m_pipe_selector_session_id;
        int m_epoll_fd;
        bool m_is_wakeuping;
        std::map<uint64_t, __SocketCtx*> m_ctxs;
		MessageLooperNotifyParam m_notify_param;
		byte_t m_recv_buf[__EPOLL_UTIL_RECV_BUF];
    };
};




#endif
S_NAMESPACE_END
#endif
