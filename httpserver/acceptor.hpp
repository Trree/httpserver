#pragma once 

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

namespace httpserver {
class Acceptor{
public:
  Acceptor(const Acceptor&) = delete;
  Acceptor& operator=(const Acceptor&) = delete;

  explicit Acceptor(int fd) {
    socklen_t peer_addr_len = sizeof(struct sockaddr_storage);
    conn_sock_ = ::accept(fd, reinterpret_cast<struct sockaddr *>(&peer_addr_), &peer_addr_len);
    if (conn_sock_.getfd() == -1) {
      perror("listen");
      exit(EXIT_FAILURE);
    }
    conn_sock_.setNonBlocking();
    conn_sock_.setNoDelay();
  }

  void getInfo() {
    int ret;
    socklen_t peer_addr_len = sizeof(struct sockaddr_storage);
    char host[NI_MAXHOST], service[NI_MAXSERV];
    ret = getnameinfo(reinterpret_cast<struct sockaddr *>(&peer_addr_),
                    peer_addr_len, host, NI_MAXHOST,
                    service, NI_MAXSERV, NI_NUMERICSERV);
    if (ret == 0) {
      std::cout << "client addr: " << host << ':' << service << '\n';
    } else {
      std::cout << "Unable to get address" << '\n';
    }
    std::cout << "the connect fd is: " << conn_sock_.getfd() << '\n';
  }

  int getfd() {
    return conn_sock_.getfd();
  }

  void swap(Acceptor& first, Acceptor& second) {
    using std::swap;
    swap(first.conn_sock_, second.conn_sock_);
    swap(first.peer_addr_, second.peer_addr_);
  }

  Acceptor(Acceptor&& other) {
    swap(*this, other);
  }

  Acceptor& operator=(Acceptor&& other) {
    swap(*this, other);
    return *this;
  }

  ~Acceptor() {
    if (conn_sock_.getfd() != -1) {
      std::cout << "Distructor Acceptor " << conn_sock_.getfd() << '\n'; 
    }
  }

private:
  Socket conn_sock_;
  struct sockaddr_storage peer_addr_;
};
} // namespace httpserver
