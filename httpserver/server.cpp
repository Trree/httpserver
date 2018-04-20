#include "server.hpp"
#include "connection.hpp"

namespace httpserver {

void HttpServer::handleEvent() {
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


}
