#ifndef HTTP_SREVER_ACCEPTOR_HPP_
#define HTTP_SREVER_ACCEPTOR_HPP_

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

  explicit Acceptor(int fd) : fd_(fd) {}

};
} // namespace httpserver

#endif // HTTP_SREVER_ACCEPTOR_HPP_
