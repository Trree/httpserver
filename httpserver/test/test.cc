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
#include <stdio.h>

void setnonblocking(int sock)
{
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

void do_use_fd(int client_fd)
{
  const char str[] = "God bless you!\n";
  if (send(client_fd,  str,  sizeof(str),  0) == -1)
      perror("send");
  close(client_fd);
}


int handle_date(std::string& ip, std::string& port) 
{ 
    const int MAX_EVENTS = 10;
    struct epoll_event ev, events[MAX_EVENTS];
    int listen_sock, conn_sock, nfds, epollfd;
    struct sockaddr_in my_addr, local;
    socklen_t addrlen;

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(std::atoi(port.c_str()));
    inet_aton(ip.c_str(), &my_addr.sin_addr);
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    bind(listen_sock, (struct sockaddr*)&my_addr, sizeof(my_addr));
    listen(listen_sock, 1024);

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

    for (;;) {
      nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
      if (nfds == -1) {
        perror("epoll_wait");
        exit(EXIT_FAILURE);
      }

      for (int n = 0; n < nfds; ++n) {
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
  

  int main()
  {
    std::string ip("0.0.0.0");
    std::string port("9999");
    handle_date(ip, port);

    return 0;
  }
