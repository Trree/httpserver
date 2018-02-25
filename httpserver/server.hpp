#ifndef HTTP_SREVER_SERVER_HPP_
#define HTTP_SREVER_SERVER_HPP_ 
#include "file.hpp"
#include "parse_uri.hpp"
#include "response.hpp"
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
#include <fstream>

namespace httpserver {

class HttpServer {
public:
  explicit HttpServer(const std::string& ip = "0.0.0.0", 
                      const std::string& port = "9999", 
                      const std::string rootdir = "/var/www/html/") 
  : ip_(ip), port_(port), rootdir_(rootdir) {
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

  void handle_read(int client_fd) {
    memset(buffer_, 0, sizeof(buffer_));
    ssize_t readlen = read(client_fd, buffer_, sizeof(buffer_));
    if (readlen <= 0) {
      perror("read failed");
    }
    ParseUri parseuri(buffer_);
    Response re(rootdir_, parseuri.getRequestUri());
    std::string response = re.getResponse();

    if (send(client_fd, response.c_str(),  response.size(),  0) == -1)
      perror("send");
    close(client_fd);
  }

  void handleEvent() {
    for (;;) {
      int nfds = epoll_wait(epollfd_, events_, 10, -1);
      for (int n = 0; n < nfds; ++n) {
        if (events_[n].data.fd == listen_fd_) {
          int conn_sock = handleAccept();
          struct epoll_event ev;
          ev.events = EPOLLIN | EPOLLET;
          ev.data.fd = conn_sock;
          if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, conn_sock,
                        &ev) == -1) {
                          perror("epoll_ctl: conn_sock");
                          exit(EXIT_FAILURE);
                        }
        } else {
          handle_read(events_[n].data.fd);
        }
      }
    }
  }

private:
  
  int handleAccept() {
    struct sockaddr_in local;
    socklen_t addrlen;
    int conn_sock = accept(listen_fd_, (struct sockaddr *) &local, &addrlen);
    if (conn_sock == -1) {
      perror("accept");
      exit(EXIT_FAILURE);
    }
    setnonblocking(conn_sock);
    return conn_sock;
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

  int listen_fd_;
  int epollfd_;
  struct epoll_event events_[10];
  char buffer_[1024];
  

  std::string ip_;
  std::string port_;
  std::string rootdir_;
};

} //namespace httpserver

#endif // HTTP_SREVER_SERVER_HPP_
