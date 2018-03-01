#ifndef HTTP_SREVER_EVENT_HPP_
#define HTTP_SREVER_EVENT_HPP_ 

#include <sys/epoll.h>

namespace httpserver {

class Event {
public:
  Event() : epollfd_(epoll_create1(0)), events_(new epoll_event[100]) {}
  Event(const Event&) = delete;
  Event& operator=(Event&) = delete;

  ~Event() {
    if (close(epollfd_) == -1) {
      std::cout << "close epollfd faied" << '\n';
    }
    epollfd_ = -1;


    if (events_) {
      delete [] events_;
    }
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
  int epollfd_ = -1;
public:
  struct epoll_event *events_;
};

}

#endif // HTTP_SREVER_EVENT_HPP_
