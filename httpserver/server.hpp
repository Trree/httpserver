#ifndef HTTP_SREVER_SERVER_HPP_
#define HTTP_SREVER_SERVER_HPP_ 

#include "connection.hpp"
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

namespace httpserver {


class HttpServer {
public:
  explicit HttpServer(const std::string& ip = "0.0.0.0", 
                      const std::string& port = "9999", 
                      const std::string rootdir = "/var/www/html/") 
  : ip_(ip), port_(port), rootdir_(rootdir) {
    bindAndListen();
    struct epoll_event ev;
    epollfd_ = epoll_create1(0);
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd_;
    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, listen_fd_, &ev) == -1) {
      perror("epoll_ctl: listen_fd_");
      exit(EXIT_FAILURE);
    }
  }
  HttpServer(const HttpServer&) = delete;
  HttpServer& operator=(HttpServer&) = delete;

  void handleEvent() {
    for (;;) {
      int nfds = epoll_wait(epollfd_, events_, 10, -1);
      for (int n = 0; n < nfds; ++n) {
        if (events_[n].data.fd == listen_fd_) {
          int conn_sock = handleAccept();
          struct epoll_event ev;
          ev.events = EPOLLIN | EPOLLET;
          ev.data.fd = conn_sock;
          if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
            perror("epoll_ctl: conn_sock");
            exit(EXIT_FAILURE);
          }
        } else {
          Connection c(events_[n].data.fd);
          c.handleConnection();
        }
      }
    }
  }

private:

  void bindAndListen() {
    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(std::atoi(port_.c_str()));
    inet_aton(ip_.c_str(), &my_addr.sin_addr);
    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd_ < 0) {
      throw std::system_error(errno, std::system_category(), "socket failed");
    }
    int ret = bind(listen_fd_, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (ret < 0) {
      throw std::system_error(errno, std::system_category(), "bind failed");
    }
    ret = listen(listen_fd_, 1024);
    if (ret < 0) {
      throw std::system_error(errno, std::system_category(), "listen failed");
    }
  }
  
  int handleAccept() {
    struct sockaddr_in client;
    socklen_t addrlen = sizeof(client);
    int conn_sock = accept(listen_fd_, (struct sockaddr *) &client, &addrlen);
    if (conn_sock == -1) {
      throw std::system_error(errno, std::system_category(), "accept failed");
    }
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

  int listen_fd_;
  int epollfd_;
  struct epoll_event events_[10];
  std::string ip_;
  std::string port_;
  std::string rootdir_;
};

} //namespace httpserver

#endif // HTTP_SREVER_SERVER_HPP_
