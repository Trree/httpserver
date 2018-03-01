#ifndef HTTP_SREVER_CONNECTION_HPP_
#define HTTP_SREVER_CONNECTION_HPP_

#include "request.hpp"
#include "response.hpp"
#include <string.h>
#include <unistd.h>

#define MAXLEN 1024

namespace httpserver {

class Connection {
public:
  Connection(int fd) : conn_fd_(fd), req_(fd) {}
  Connection(const Connection&) = delete;
  Connection& operator=(Connection&) = delete;
  ~Connection() {
    close(conn_fd_);
    conn_fd_ = -1;
  }
  
  bool handleConnection() {
    req_.handleRead();
    std::string response = req_.handleResponse();
    req_.handleWrite(response);
    return true;
  }
private:
  int conn_fd_ = -1;
  Request req_;
};


} // namespace httpserver

#endif // HTTP_SREVER_CONNECTION_HPP__

