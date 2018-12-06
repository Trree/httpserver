#ifndef HTTP_SREVER_CONNECTION_HPP_
#define HTTP_SREVER_CONNECTION_HPP_

#include "request.hpp"
#include "buffer.hpp"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <memory>

#define MAXLEN 4096

namespace httpserver {

class ConnectionManager;

class Connection : public std::enable_shared_from_this<Connection> {
public:
  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;
  
  explicit Connection(int fd, ConnectionManager& cm) 
  : fd_(fd), 
    connections_manager_(cm) {}
  ~Connection() {
    std::cout << "destruction connection " << fd_ << '\n';
    close(fd_);
    fd_ = -1;
  }

  void start(); 
  void stop(); 
  int getFd();
  void setFd(int fd);

  int handleWrite(std::string response);
  bool handleRead();
  int Read(char* buffer, size_t size); 
  bool isComplete(std::string header);

private:
  int fd_{-1};
  Buffer buffer_;
  ConnectionManager &connections_manager_;
};


} // namespace httpserver

#endif // HTTP_SREVER_CONNECTION_HPP__

