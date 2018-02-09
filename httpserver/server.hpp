#ifndef HTTP_SREVER_SERVER_HPP_
#define HTTP_SREVER_SERVER_HPP_

#include "file.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <string>
#include <iostream>
#include <string.h>
#include <cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

namespace httpserver {

class HttpServer {
public:
  explicit HttpServer(const std::string& ip = "0.0.0.0", const std::string& port = "8888", const std::string rootdir = "/var/www") : ip_(ip), port_(port), rootdir_(rootdir) {
    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(std::atoi(port_.c_str()));
    inet_aton(ip_.c_str(), &my_addr.sin_addr);
    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    bind(listen_fd_, (struct sockaddr*)&my_addr, sizeof(my_addr));
    listen(listen_fd_, 1024);

    struct epoll_event ev;
    epollfd_ = epoll_create1(0);
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd_;
    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, listen_fd_, &ev) == -1) {
      perror("epoll_ctl: listen_fd_");
      exit(EXIT_FAILURE);
    }
  }

  void setnonblocking(int sock){
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

  void do_use_fd(int client_fd) {
    const char str[] = "God bless you!\n";
    if (send(client_fd,  str,  sizeof(str),  0) == -1)
    perror("send");
    close(client_fd);
  }

  void handleEvent() {
    struct sockaddr_in local;
    socklen_t addrlen;
    int  conn_sock;
    for (;;) {
      int nfds = epoll_wait(epollfd_, events_, 10, -1);
      for (int n = 0; n < nfds; ++n) {
        if (events_[n].data.fd == listen_fd_) {
          conn_sock = accept(listen_fd_,
                             (struct sockaddr *) &local, &addrlen);
          if (conn_sock == -1) {
            perror("accept");
            exit(EXIT_FAILURE);

          }
          setnonblocking(conn_sock);
          struct epoll_event ev;
          ev.events = EPOLLIN | EPOLLET;
          ev.data.fd = conn_sock;
          if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, conn_sock,
                        &ev) == -1) {
                          perror("epoll_ctl: conn_sock");
                          exit(EXIT_FAILURE);
                        }
        } else {
          do_use_fd(events_[n].data.fd);
        }
      }
    }
  }

private:
  int listen_fd_;
  int epollfd_;
  struct epoll_event events_[10];

  std::string ip_;
  std::string port_;
  std::string rootdir_;
};

} //namespace httpserver

#endif // HTTP_SREVER_SERVER_HPP_
