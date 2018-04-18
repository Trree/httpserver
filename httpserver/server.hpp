#ifndef HTTP_SREVER_SERVER_HPP_
#define HTTP_SREVER_SERVER_HPP_ 

#include "connection_manager.hpp"
#include "socket.hpp"
#include "event.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h> 
#include <exception>

namespace httpserver {
 
using connection_ptr = std::shared_ptr<Connection>;

class HttpServer {
public:
  HttpServer(const HttpServer&) = delete;
  HttpServer& operator=(const HttpServer&) = delete;
  
  explicit HttpServer(const std::string& ip = "::", 
                      const std::string& port = "9999")
  : socket_(ip, port), event_(){
    socket_.listen();
    event_.add(socket_.getfd(), 0, EPOLLIN);
  }

  ~HttpServer() {}

  void handleEvent() {
    auto timer = [&]() { 
      std::cout << "Start timer ..." << '\n';
      connections_manager_.regularClean();
    };
    for (;;) {
      int nfds = epoll_wait(event_.getEpollFd(), event_.events_, 100, 5000);
      if (nfds < 0) {
        std::cout << "epoll_wait() error" << errno << strerror(errno) << '\n';
        continue;
      }
      if (nfds == 0) {
        timer();
      }
      for (int n = 0; n < nfds; ++n) {
        auto event = event_.events_[n];
        auto revents = event.events;
        if (revents & (EPOLLERR | EPOLLHUP)) {
          std::cout << "epoll_wait() error on fd:" << event.data.fd << " ev: " << revents << '\n';
          revents |= EPOLLIN | EPOLLOUT;
        }
        else if (revents & EPOLLRDHUP) {
          std::cout << "EPOLLRDHUP" << '\n';
        }

        try {
          if (revents & EPOLLIN) {
            if (event.data.u64  == 0) {
              int fd = handleAccept(socket_.getfd());
              uint32_t num = connections_manager_.start(fd);
              event_.add(fd, num, EPOLLIN|EPOLLET);
            }
            else {
              auto conn = connections_manager_.getConnection(event.data.u64);
              conn->start();
            }
          }
          else if (revents & EPOLLOUT) {
            auto conn = connections_manager_.getConnection(event.data.u64);
            conn->start();
            std::cout << "epoll_wait epollout: handle" << '\n' ;
          }
        } catch (const std::exception& e) {
          std::cout << e.what() << '\n';
        }
      }
    }
  }

private:

  int handleAccept(int listen_fd) {
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len = sizeof(struct sockaddr_storage);
    int conn_sock = accept(listen_fd, (struct sockaddr *) &peer_addr, &peer_addr_len);
    if (conn_sock == -1) {
      perror("listen");
      exit(EXIT_FAILURE);
    }
     
    int ret;
    char host[NI_MAXHOST], service[NI_MAXSERV];
    ret = getnameinfo((struct sockaddr *) &peer_addr,
                    peer_addr_len, host, NI_MAXHOST,
                    service, NI_MAXSERV, NI_NUMERICSERV);
    if (ret == 0) {
      std::cout << "client addr: " << host << ':' << service << '\n';
    } else {
      std::cout << "Unable to get address" << '\n';
    }
    std::cout << "the connect fd is: " << conn_sock << '\n';

    setNonBlocking(conn_sock);

    return conn_sock;
  }

  void setNonBlocking(int sock){
    int opts;
    if ((opts = fcntl(sock, F_GETFL)) < 0) {
      printf("GETFL %d failed", sock);
      exit(1);
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(sock, F_SETFL, opts) < 0) {
      printf("SETFL %d failed", sock);
      exit(1);
    }
  }

  Socket socket_;
  Event event_;
  ConnectionManager connections_manager_;
};

} //namespace httpserver

#endif // HTTP_SREVER_SERVER_HPP_
