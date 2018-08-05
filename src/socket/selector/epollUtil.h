#ifndef S_SOCKET_EPOLLUTIL_H_
#define S_SOCKET_EPOLLUTIL_H_
#include "socketSelector.h"
S_NAMESPACE_BEGIN
//#define S_OS_LINUX
#if  defined(S_OS_LINUX) || defined(S_OS_ANDROID)
#include <sys/epoll.h>



class EpollUtil
{
public:
    class EpollRun : public IThreadRun
    {
    public:
        EpollRun()
        {
            m_is_exit = false;
            m_is_wakeuping = false;
            m_pipe[0] = 0;
            m_pipe[1] = 0;
            
            if (pipe(m_pipe) != 0)
            {
                slog_e("EpollRun:: fail to pipe");
                return;
            }

            m_epoll_fd = epoll_create(100 * 1000);
            if(m_epoll_fd == -1)
            {
                slog_e("EpollRun:: fail to epoll_create");
                return;
            }

			if (!__addFdEvent(m_pipe[0], EPOLLIN))
			{
				slog_e("EpollRun:: fail to __addFdEvent");
				return;
			}
        }
        
        virtual void run() override 
        {
            slog_i("EpollRun:: run");
            __onRun();
            
            ScopeMutex __l(m_mutex);
            __releaseAll();
            slog_i("EpollRun:: exit");
        }
        
		virtual void stop() override 
        {
            ScopeMutex __l(m_mutex);
            if(m_is_exit)
                return;
            m_is_exit = true;
            slog_i("EpollRun:: stop");
            __wakeup();
        }
        
        bool addAcceptSocket(socket_t socket, uint64_t session_id)
        {
            ScopeMutex __l(m_mutex);
            if(m_is_exit)
                return false;
			
            if(is_map_contain_key(m_ctxs, socket))
                return false;
            __SocketCtx* ctx = new __SocketCtx(socket, true, session_id);
            m_ctxs[socket] = ctx;
            if(!__addFdEvent(socket, EPOLLIN))
			{
				slog_e("EpollRun:: addAcceptSocket fail to __addFdEvent");
				return false;
			}
			
            __wakeup();
            return true;
        }

        bool addTranSocket(socket_t socket, uint64_t session_id)
        {
            ScopeMutex __l(m_mutex);
            if(m_is_exit)
                return false;
			
            if(is_map_contain_key(m_ctxs, socket))
                return false;
            __SocketCtx* ctx = new __SocketCtx(socket, false, session_id);
            m_ctxs[socket] = ctx;
            if(!__addFdEvent(socket, EPOLLIN))
			{
				slog_e("EpollRun:: addTranSocket fail to __addFdEvent");
				return false;
			}
            
			slog_i("EpollRun:: add tran");
            __wakeup();
            return true;
        }

        bool postSendCmd(socket_t socket, uint64_t session_id, uint64_t send_cmd_id, const byte_t* data, size_t data_len)
        {
            ScopeMutex __l(m_mutex);
            if(m_is_exit)
                return false;
			
            __SocketCtx* ctx = get_map_element_by_key(m_ctxs, socket);
            if(ctx == nullptr)
                return false;
            __SendCmd* cmd = new __SendCmd(send_cmd_id, data, data_len);
            ctx->m_send_cmds.push_back(cmd);
			if(ctx->m_send_cmds.size() == 1)
			{
				if(!__modFdEvent(socket, EPOLLIN | EPOLLOUT))
				{
					slog_e("EpollRun:: postSendCmd fail to __modFdEvent");
					return false;
				}
			}
           
		   slog_i("EpollRun:: post send");
            __wakeup();
            return true;
        }

	    bool postSendToCmd(socket_t socket, uint64_t session_id, uint64_t send_cmd_id, Ip to_ip, uint32_t to_port, const byte_t* data, size_t data_len)
        {
            ScopeMutex __l(m_mutex);
            if(m_is_exit)
                return false;
			
            __SocketCtx* ctx = get_map_element_by_key(m_ctxs, socket);
            if(ctx == nullptr)
                return false;
            __SendToCmd* cmd = new __SendToCmd(send_cmd_id, data, data_len, to_ip, to_port);
            ctx->m_send_to_cmds.push_back(cmd);
			if(ctx->m_send_to_cmds.size() == 1)
			{
				if(!__modFdEvent(socket, EPOLLIN | EPOLLOUT))
				{
					slog_e("EpollRun:: postSendToCmd fail to __modFdEvent");
					return false;
				}
			}
			
			slog_i("EpollRun:: post sendto");
            __wakeup();
            return true;
        }
        
		
		
    private:
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
            __SocketCtx() { m_socket = INVALID_SOCKET; m_is_accept_socket = false; m_session_id = 0; }
            __SocketCtx(socket_t s, bool is_accept_socket, uint64_t session_id) { m_socket = s; m_is_accept_socket = is_accept_socket; m_session_id = session_id;}
            socket_t m_socket;
            bool m_is_accept_socket;
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
                if (event_count == -1)
                {
                    slog_e("EpollRun:: onRun fail to epoll_wait");
                    return;
                }

                ScopeMutex __l(m_mutex);
				if(m_is_exit)
					return;
                m_is_wakeuping = false;
				
                for (int i = 0 ; i < event_count; i++) 
                {
					epoll_event& ev = evs[i];
					
					if(ev.data.fd == m_pipe[1])
					{
						char b;
						::read(m_pipe[0], &b, 1);
						slog_i("EpollRun:: onRun recv signal");
						continue;
					}
					
					__onSocketEvent(&ev);
                }
            }
        }
		
		void __onSocketEvent(epoll_event* ev)
		{
			__SocketCtx* ctx = get_map_element_by_key(m_ctxs, ev->data.fd);
			if(ctx == nullptr)
			{
				__delFdEvent(ev.data.fd);
				return;
			}
			
			if(ctx->m_is_accept_socket)
			{
				if(ev->events & EPOLLERR || ev->events & EPOLLHUP)
				{
					// release resource and post msg
					return;
				}
				
				if(ev->events & EPOLLIN)
				{
					// accept and post msg
				}
			}
			else
			{
				if(ev->events & EPOLLERR || ev->events & EPOLLHUP)
				{
					// release resource and post msg
					return;
				}
				
				if(ev->events & EPOLLIN)
				{
					// recv and post msg
				}
				
				if(ev->events & EPOLLOUT)
				{
					// send and post msg
				}
			}
		}
        
        void __wakeup()
        {
            if(m_is_wakeuping)
                return;
            m_is_wakeuping = true;
            slog_i("EpollRun:: __wakeup");
            write(m_pipe[1], "0", 1);		
        }
        
        void __releaseAll()
        {
		    if (m_pipe[0] != 0)
		    {
                close(m_pipe[0]);
                close(m_pipe[1]);
                m_pipe[0] = 0;
                m_pipe[1] = 0;
            }
            if(m_epoll_fd != -1)
            {
                close(m_epoll_fd);
                m_epoll_fd = -1;
            }
        }
		
		bool __addFdEvent(int fd, int events)
		{
            epoll_event ev;
            ev.data.fd = fd;
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
            ev.data.fd = fd;
            ev.events = 0;
			return epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, &ev) != -1;
		}
		
		bool __modFdEvent(int fd, int events)
		{
            epoll_event ev;
            ev.data.fd = fd;
            ev.events = events;
			return epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, fd, &ev) != -1;
		}
        
		
		
        bool m_is_exit;
        Mutex m_mutex;
	    int m_pipe[2];
        int m_epoll_fd;
        bool m_is_wakeuping;
        std::map<socket_t, __SocketCtx*> m_ctxs;
    };
};




#endif
S_NAMESPACE_END
#endif
