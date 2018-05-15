#pragma once 

#include "inetaddress.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <string.h> 
#include <iostream>

namespace httpserver {
class Socket {
public:
  Socket(const Socket&) = delete;
  Socket& operator=(const Socket&) = delete;

  Socket() : sockfd_(-1) {}
  Socket(int fd) : sockfd_(fd) {}
  
  void bindAddress(const std::string& addr, const std::string& port) {
    AddrinfoGuard result(handleInetAddress(addr, port));
    struct addrinfo *rp;

    for (rp = result.info_; rp != NULL; rp = rp->ai_next) {
      sockfd_ = ::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
      if (sockfd_ == -1)
        continue;
      setReuseAddr();

      if (::bind(sockfd_ , rp->ai_addr, rp->ai_addrlen) == 0)
        break;                  /* Success */
      close(sockfd_);
    }

    if (rp == NULL) {               /* No address succeeded */
      fprintf(stderr, "Could not bind\n");
      exit(EXIT_FAILURE);
    }
  }

  void swap(Socket& first, Socket& second) {
    using std::swap;
    swap(first.sockfd_, second.sockfd_);
  }

  //Socket(Socket&&) = default;
  //Socket& operator=(Socket&&) = default;
  
  
  Socket(Socket&& other) noexcept : sockfd_(std::move(other.sockfd_)) {
    other.sockfd_ = -1;
  }

  Socket& operator=(Socket&& other) noexcept {
    swap(*this, other);
    return *this;
  }

  void listen() {
    int ret;
    ret = ::listen(sockfd_, 1024);
    if (ret < 0) {
      perror("listen");
      exit(EXIT_FAILURE);
    }
  }

  int getfd() {
    return sockfd_;
  }

  ~Socket() {
    if (sockfd_ != -1) {
      std::cout << "destruction Socket " << sockfd_ << '\n';
      close(sockfd_);
      sockfd_ = -1;
    }
  }

  void setNonBlocking(){
    int opts;
    if ((opts = fcntl(sockfd_, F_GETFL)) < 0) {
      printf("GETFL %d failed", sockfd_);
      exit(1);
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(sockfd_, F_SETFL, opts) < 0) {
      printf("SETFL %d failed", sockfd_);
      exit(1);
    }
  }
  
  void setReuseAddr(){
    int reuse = 1;
    int ret = setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if (ret < 0) {
      printf("Set SO_REUSEADDR %d failed", sockfd_);
      exit(1);
    }
  }
  void setNoDelay(){
    int tcp_nodelay = 0;
    int ret = setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, (const void*)&tcp_nodelay, sizeof(int));
    if (ret < 0) {
      printf("Set TCP_NODELAY %d failed", sockfd_);
      exit(1);
    }
  }

private:
  int sockfd_{-1};
};

} // namespace httpserver

