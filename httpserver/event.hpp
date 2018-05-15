#pragma once 

#include <sys/epoll.h>

namespace httpserver {

class Event {
public:
  Event(const Event&) = delete;
  Event& operator=(const Event&) = delete;

  Event() : epollfd_(epoll_create1(0)), events_(new epoll_event[100]) {}
  ~Event() {
    if (close(epollfd_) == -1) {
      std::cout << "close epollfd faied" << '\n';
    }
    epollfd_ = -1;

    if (events_) {
      delete [] events_;
    }
  }

  void add(int fd, uint32_t num, uint32_t event) {
    struct epoll_event ev;
    ev.events = event;
    ev.data.u64 = num;
    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
      perror("epoll_ctl: fd_");
      exit(EXIT_FAILURE);
    }
  }

  int getEpollFd() const {
    return epollfd_;
  }
private:
  int epollfd_{-1};
public:
  struct epoll_event *events_;
};

}
