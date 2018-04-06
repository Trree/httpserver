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
#include <chrono>

#define MAXLEN 4096

namespace httpserver {

class ConnectionManager;

class Connection : public std::enable_shared_from_this<Connection> {
public:
  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;
  
  explicit Connection(int fd, uint64_t key, ConnectionManager& cm) 
  : fd_(fd), 
    key_(key),
    begin_time_(std::chrono::system_clock::now()),
    connections_manager_(cm) {}
  ~Connection() {
    std::cout << "destruction connection " << fd_ << '\n';
    close(fd_);
    fd_ = -1;
  }

  void start(); 
  void stop(); 
  int getFd();
  int getKey();
  void setFd(int fd);

  int handleWrite(std::string response);
  bool handleRead();
  int Read(char* buffer, size_t size); 
  bool isComplete(std::string header);

private:
  int fd_{-1};
  uint64_t key_;
  Buffer buffer_;
  std::chrono::system_clock::time_point begin_time_;
  ConnectionManager &connections_manager_;
};


} // namespace httpserver

#endif // HTTP_SREVER_CONNECTION_HPP__

