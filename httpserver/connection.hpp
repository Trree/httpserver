#pragma once 

#include "socket.hpp"
#include "buffer.hpp"
#include "file.hpp"
#include "acceptor.hpp"
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
  Connection(const Connection&) = delete; Connection& operator=(const Connection&) = delete;
 
  enum class StatusType {
    closed,
    established,
    read,
    write
  } ; 
 
  Connection(Acceptor&& acceptor, uint64_t key, ConnectionManager& cm) 
  : acceptor_(std::move(acceptor)), 
    key_(key),
    status_(StatusType::established),
    keepalive_(false),
    end_time_(std::chrono::high_resolution_clock::now()),
    connections_manager_(cm) {}
  ~Connection() {
    std::cout << "destruction connection " << acceptor_.getfd() << '\n';
  }

  void start(); 
  void stop(); 
  int getKey();
  std::chrono::high_resolution_clock::time_point getExpiredTime() {
    return end_time_ + std::chrono::seconds(keepalivetimeout_);
  }
  void setStatus(StatusType status) {
    status_ = status;
  }
  StatusType getStatus() {
    return status_;
  }

  int getfd() {
    return acceptor_.getfd();
  }

  int handleWrite(std::string response);
  bool handleRead();
  int Read(char* buffer, size_t size); 
  void setKeepalive(bool on) {
    keepalive_ = on;
  }
  bool getKeepalive() {
    return keepalive_;
  }

  void setFile(std::string filename) {
    File file(filename);
    filechain_.push(std::move(file));
  }

  int sendfile() {
    while (!filechain_.empty()) {
      filechain_.pop(acceptor_.getfd()); 
    }
    if (filechain_.empty() && !getKeepalive()) {
      stop();
    }
    return 0;
  }

private:

  Acceptor acceptor_;
  uint64_t key_;
  Buffer buffer_;
  FileChain filechain_;
  StatusType status_{StatusType::closed};
  bool keepalive_;
  int keepalivetimeout_ = 15;
  std::chrono::high_resolution_clock::time_point end_time_;
  ConnectionManager &connections_manager_;
};


} // namespace httpserver


