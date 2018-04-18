#ifndef HTTP_SREVER_CONNECTION_HPP_
#define HTTP_SREVER_CONNECTION_HPP_

#include "buffer.hpp"
#include "http/request.hpp"
#include <unistd.h>
#include <string>
#include <memory>
#include <chrono>
#include <iostream>

#define MAXLEN 4096

namespace httpserver {

class ConnectionManager;

class Connection : public std::enable_shared_from_this<Connection> {
public:
  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;
 
  enum class StatusType {
    closed,
    established,
    read,
    write
  } status; 
 
  explicit Connection(int fd, uint64_t key, ConnectionManager& cm) 
  : fd_(fd), 
    key_(key),
    status_(StatusType::established),
    keepalive_(false),
    end_time_(std::chrono::high_resolution_clock::now()),
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
  int getKey();
  std::chrono::high_resolution_clock::time_point getExpiredTime() {
    return end_time_ + std::chrono::seconds(keepalivetimeout_);
  }
  void setStatus(StatusType status) {
    status_ = status;
  }

  int handleWrite(std::string response);
  bool handleRead();
  int Read(char* buffer, size_t size); 
  bool isComplete(std::string header);

private:

  int fd_{-1};
  uint64_t key_;
  Buffer buffer_;
  StatusType status_{StatusType::closed};
  bool keepalive_;
  int keepalivetimeout_ = 15;
  std::chrono::high_resolution_clock::time_point end_time_;
  ConnectionManager &connections_manager_;
};


} // namespace httpserver

#endif // HTTP_SREVER_CONNECTION_HPP__

