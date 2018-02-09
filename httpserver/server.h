#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>

class HttpServer {
public:
  HttpServer(const std::string& ip, const std::string& port, const std::string rootdir) : ip_(ip), port_(port), rootdir_(rootdir) {
    struct sockaddr_in my_addr, peer_addr;
    socklen_t peer_addr_size;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sun_family = AF_INET;
    my_addr.sin_port = htons(port_.atoi());
    inet_pton(AF_INET, ip_.c_str(), &my_addr.sin_addr);
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    auto ret = bind(fd_, (struct sockaddr*)&my_addr, sizeof(my_addr));
    auto ret = listen(fd_, 1024);
  }
 
  int epoll() {
    const MAX_EVENTS = 10
    struct epoll_event ev, events[MAX_EVENTS];
    int listen_sock, conn_sock, nfds, epollfd;

    epollfd = epoll_create1(0);
    if (epollfd == -1) {
      perror("epoll_create1");
      exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
      perror("epoll_ctl: listen_sock");
      exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
      perror("epoll_ctl: listen_sock");
      exit(EXIT_FAILURE);
    }

    for (;;) {
      nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
      if (nfds == -1) {
        perror("epoll_wait");
        exit(EXIT_FAILURE);
      }

      for (n = 0; n < nfds; ++n) {
        if (events[n].data.fd == listen_sock) {
          conn_sock = accept(listen_sock,
                             (struct sockaddr *) &local, &addrlen);
          if (conn_sock == -1) {
            perror("accept");
            exit(EXIT_FAILURE);

          }
          setnonblocking(conn_sock);
          ev.events = EPOLLIN | EPOLLET;
          ev.data.fd = conn_sock;
          if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,
                        &ev) == -1) {
                          perror("epoll_ctl: conn_sock");
                          exit(EXIT_FAILURE);
                        }
        } else {
          do_use_fd(events[n].data.fd);
        }
      }
    }
  }   
  
private:
  File fd_;

  std::string ip_("0.0.0.0");
  std::string port_("80");
  std::string rootdir_("/var/www");
}


       int bind(int sockfd, const struct sockaddr *addr,
                               socklen_t addrlen);

