#ifndef HTTP_SREVER_COMMON_HPP_
#define HTTP_SREVER_COMMON_HPP_ 

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

namespace httpserver {

Socket setSocket(const std::string& addr, const std::string& port) {
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
    Socket socket = socket(rp->ai_family, rp->ai_socktype,
                 rp->ai_protocol);
    if (socket.getfd()_ == -1)
      continue;
    socket.setReuseAddr();

    if (bind(socket.getfd(), rp->ai_addr, rp->ai_addrlen) == 0)
      break;                  /* Success */

    close(socket);
  }

  if (rp == NULL) {               /* No address succeeded */
    fprintf(stderr, "Could not bind\n");
    exit(EXIT_FAILURE);
  }
  freeaddrinfo(result);

  return socket;
}

void listen(Socket socket) {
  int ret;
  ret = ::listen(socket.getfd(), 1024);
  if (ret < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
}

} // namespace httpserver

#endif // HTTP_SREVER_COMMON_HPP_
