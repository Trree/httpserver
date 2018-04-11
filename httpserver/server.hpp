#ifndef HTTP_SREVER_SERVER_HPP_
#define HTTP_SREVER_SERVER_HPP_ 

#include "connection.hpp"
#include "connection_manager.hpp"
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
#include <memory>
#include <map>
#include <exception>

namespace httpserver {
 
using connection_ptr = std::shared_ptr<Connection>;

class HttpServer {
public:
  HttpServer(const HttpServer&) = delete;
  HttpServer& operator=(const HttpServer&) = delete;
  
  explicit HttpServer(const std::string& ip = "0.0.0.0", 
                      const std::string& port = "9999", 
                      const std::string rootdir = "/var/www/html/") 
  : ip_(ip), port_(port), rootdir_(rootdir), event_(){
    bindAndListen();
    event_.add(listen_fd_, 0, EPOLLIN);
  }
  ~HttpServer() {
    close(listen_fd_);
    listen_fd_ = -1;
  }

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
              int fd = handleAccept(listen_fd_);
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

  void bindAndListen() {
    int ret;
    int reuse;
    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(std::atoi(port_.c_str()));
    inet_aton(ip_.c_str(), &my_addr.sin_addr);
    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd_ < 0) {
      perror("socket");
      exit(EXIT_FAILURE);
    }
    reuse = 1;
    ret = setsockopt( listen_fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    ret = bind(listen_fd_, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (ret < 0) {
      perror("bind");
      exit(EXIT_FAILURE);
    }
    ret = listen(listen_fd_, 1024);
    if (ret < 0) {
      perror("listen");
      exit(EXIT_FAILURE);
    }
  }
 
  int handleAccept(int listen_fd) {
    struct sockaddr_in client;
    socklen_t addrlen = sizeof(client);
    int conn_sock = accept(listen_fd, (struct sockaddr *) &client, &addrlen);
    if (conn_sock == -1) {
      perror("listen");
      exit(EXIT_FAILURE);
    }

    char clntName[INET6_ADDRSTRLEN];
    char portName[6];
    if(getnameinfo((const sockaddr*)&client, sizeof(client),
                   clntName, sizeof(clntName),
                   portName, sizeof(portName),
                   NI_NUMERICHOST|NI_NUMERICSERV)==0){
      std::cout << "client addr: " << clntName << ':' << portName << '\n';
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

  std::string ip_;
  std::string port_;
  std::string rootdir_;
  int listen_fd_ = -1;
  Event event_;
  ConnectionManager connections_manager_;
};

} //namespace httpserver

#endif // HTTP_SREVER_SERVER_HPP_
