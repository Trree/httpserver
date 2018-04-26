#ifndef HTTP_SREVER_SOCKET_HPP_
#define HTTP_SREVER_SOCKET_HPP_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
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

  Socket() : fd_(-1) {}
  Socket(int fd) : fd_(fd) {}
  explicit Socket(const std::string& addr, const std::string& port) {
    int ret;
    struct addrinfo hints;
    struct addrinfo *result;
    struct addrinfo *rp;
    memset(&hints, 0, sizeof(struct addrinfo));
    bool ipv6 = true; 
    if (ipv6) {
      hints.ai_family = AF_INET6;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_protocol = 0;
      hints.ai_canonname = NULL;
      hints.ai_addr = NULL;
      hints.ai_next = NULL;
      ret = getaddrinfo(addr.c_str(), port.c_str(),&hints, &result);
      if (ret != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        exit(EXIT_FAILURE);
      }
    }
    else {
      hints.ai_family = AF_INET;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_protocol = 0;
      hints.ai_canonname = NULL;
      hints.ai_addr = NULL;
      hints.ai_next = NULL;
      ret = getaddrinfo(addr.c_str(), port.c_str(),&hints, &result);
      if (ret != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        exit(EXIT_FAILURE);
      }
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
      fd_ = socket(rp->ai_family, rp->ai_socktype,
                   rp->ai_protocol);
      if (fd_ == -1)
        continue;
      int reuse = 1;
      ret = setsockopt( fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

      if (bind(fd_ , rp->ai_addr, rp->ai_addrlen) == 0)
        break;                  /* Success */
      close(fd_);
    }

    if (rp == NULL) {               /* No address succeeded */
      fprintf(stderr, "Could not bind\n");
      exit(EXIT_FAILURE);
    }
    freeaddrinfo(result);
  }

  void swap(Socket& first, Socket& second) {
    using std::swap;
    swap(first.fd_, second.fd_);
  }

  Socket(Socket&& other) : fd_(other.fd_) {
    other.fd_ = -1;
  }

  Socket& operator=(Socket&& other) {
    swap(*this, other);
    return *this;
  }

  void listen() {
    int ret;
    ret = ::listen(fd_, 1024);
    if (ret < 0) {
      perror("listen");
      exit(EXIT_FAILURE);
    }
  }

  int getfd() {
    return fd_;
  }

  ~Socket() {
    std::cout << "destruction Socket " << fd_ << '\n';
    close(fd_);
    fd_ = -1;
  }

  void setNonBlocking(){
    int opts;
    if ((opts = fcntl(fd_, F_GETFL)) < 0) {
      printf("GETFL %d failed", fd_);
      exit(1);
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(fd_, F_SETFL, opts) < 0) {
      printf("SETFL %d failed", fd_);
      exit(1);
    }
  }
  
  void setReuseAddr(){
    int reuse = 1;
    int ret = setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if (ret < 0) {
      printf("Set SO_REUSEADDR %d failed", fd_);
      exit(1);
    }
  }

private:
  int fd_;
};

} // namespace httpserver

#endif // HTTP_SREVER_SOCKET_HPP_
