#include "server.hpp"
#include "connection.hpp"
#include "socket.hpp"
#include <sys/epoll.h>

namespace httpserver {

Socket HttpServer::handleAccept() {
  struct sockaddr_storage peer_addr;
  socklen_t peer_addr_len = sizeof(struct sockaddr_storage);
  Socket conn_sock = accept(socket_.getfd(), (struct sockaddr *) &peer_addr, &peer_addr_len);
  if (conn_sock.getfd() == -1) {
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
  std::cout << "the connect fd is: " << conn_sock.getfd() << '\n';

  conn_sock.setNonBlocking();

  return conn_sock;
}

void HttpServer::handleEvent() {
  auto timer = [&]() { 
    std::cout << "Start timer ..." << '\n';
    connections_manager_.regularClean();
  };
  for (;;) {
    int nfds = epoll_wait(event_.getEpollFd(), event_.events_, 100, 15000);
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
            Socket socket= handleAccept();
            int fd = socket.getfd();
            uint64_t num = connections_manager_.start(std::move(socket));
            event_.add(fd, num, EPOLLIN| EPOLLOUT |EPOLLET);
          }
          else {
            auto conn = connections_manager_.getConnection(event.data.u64);
            conn->start();
          }
        }
        else if (revents & EPOLLOUT) {
          auto conn = connections_manager_.getConnection(event.data.u64);
          std::cout << "epoll_wait epollout: handle " << conn->getfd() << " : " << event.data.u64 << '\n' ;
          conn->sendfile();
        }
      } catch (const std::exception& e) {
        std::cout << e.what() << '\n';
      }
    }
  }
}


}
