#ifndef HTTP_SREVER_EVENT_HPP_
#define HTTP_SREVER_EVENT_HPP_ 

#include <sys/epoll.h>

namespace httpserver {

class Event {
public:
  struct epoll_event events_[100];
  
  Event() {
    epollfd_ = epoll_create1(0);
  }

  void add(int fd) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = fd;
    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
      perror("epoll_ctl: fd_");
      exit(EXIT_FAILURE);
    }
  }

  int getEpollFd() const {
    return epollfd_;
  }

private:
  int epollfd_;
};

}

#endif // HTTP_SREVER_EVENT_HPP_
