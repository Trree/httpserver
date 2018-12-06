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
    event_.add(listen_fd_, EPOLLIN);
  }
  ~HttpServer() {
    close(listen_fd_);
    listen_fd_ = -1;
  }

  void handleEvent() {
    for (;;) {
      try {
        int nfds = epoll_wait(event_.getEpollFd(), event_.events_, 100, -1);
        if (nfds < 0) {
          std::cout << "epoll_wait() error" << errno << strerror(errno) << '\n';
          continue;
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
          
          if (revents & EPOLLIN) {
            if (event.data.fd  == listen_fd_) {
              int fd = handleAccept(listen_fd_);
              auto connptr = std::make_shared<Connection>(fd, connections_manager_);
              connections_manager_.start(connptr);
              struct epoll_event ev;
              ev.events = EPOLLIN | EPOLLET;
              ev.data.ptr = static_cast<void*>(connptr.get());
              if (epoll_ctl(event_.getEpollFd(), EPOLL_CTL_ADD, fd, &ev) == -1) {
                std::cout << "epoll_ctl failed. fd is " << fd << '\n';
                perror("epoll_ctl: fd_");
                exit(EXIT_FAILURE);
              }
              continue;
            }
            // connctions_manager_ manage the lifetime of connection.
            // when earse the connecion. The connection would distruction.
            auto conn = static_cast<Connection *>(event.data.ptr);
            conn->start();
          }
          else if (revents & EPOLLOUT) {
            std::cout << "epoll_wait epollout: handle" << '\n' ;
          }
        }
      }
      catch (const std::exception& e) {
        std::cout << e.what() << '\n';
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
      throw std::system_error(errno, std::system_category(), "socket failed");
    }
    reuse = 1;
    ret = setsockopt( listen_fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    ret = bind(listen_fd_, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (ret < 0) {
      throw std::system_error(errno, std::system_category(), "bind failed");
    }
    ret = listen(listen_fd_, 1024);
    if (ret < 0) {
      throw std::system_error(errno, std::system_category(), "listen failed");
    }
  }
 
  int handleAccept(int listen_fd) {
    struct sockaddr_in client;
    socklen_t addrlen = sizeof(client);
    int conn_sock = accept(listen_fd, (struct sockaddr *) &client, &addrlen);
    if (conn_sock == -1) {
      throw std::system_error(errno, std::system_category(), "accept failed");
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
