#ifndef HTTP_SREVER_CONNECTION_HPP_
#define HTTP_SREVER_CONNECTION_HPP_

#include "request.hpp"
#include "buffer.hpp"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>

#define MAXLEN 4096
#define MMAXLEN 8192

namespace httpserver {

class Connection {
public:
  Connection(int fd) : fd_(fd) {}
  Connection(const Connection&) = delete;
  Connection& operator=(Connection&) = delete;
  ~Connection() {
    close(fd_);
    fd_ = -1;
  }
  
  bool handleConnection() {
    if (handleRead()) {
      Request req(buffer_.getBuffer());
      std::string response = req.handleRequest();
      int wlen = handleWrite(response);
      if (-1 == wlen) {
        std::cout << "send errno: " << errno << ", " << strerror(errno) << '\n';
        return false;
      }
    }
    else {
      return false;
    }
    return true;
  }

  int handleWrite(std::string response) {
    size_t wlen = 0;
    int left = response.size();
    wlen = send(fd_, response.c_str(), left, 0);
    std::cout << "send: fd:" << fd_ << ' ' << wlen << " of" << ' ' << left << '\n'; 

    return wlen;
  }
  
  bool handleRead() {
    size_t len = MAXLEN;
    char buffer[MAXLEN] = {0};
    int size = Read(buffer, len);
    if (-1 == size) {
      std::cout << "recv errno: " << errno << ", " << strerror(errno) << '\n';
      return false;
    }
    buffer_.assignBuffer(buffer, size);
    if (!checkComplete(buffer_.getBuffer())) {
      return false;
    }
    return true;
  }

  int Read(char* buffer, size_t size) {
    ssize_t n = 0;
    do {
      n = recv(fd_, buffer, size, 0);
      std::cout << "recv: fd:" << fd_ << ' ' << n << " of" << ' ' << size << '\n'; 

      if (n == 0) {
        return 0;
      }

      if (n > 0) {
        if ((size_t) n < size) {
          return n;
        }
      }
    } while (errno == EINTR);
    
    return n;
  }

private:
  bool isComplete(std::string header) {
    auto n = header.find("\r\n\r\n");
    if (n == std::string::npos) {
      return false;
    }
    return true;
  }

  bool checkComplete(const std::string header) {
    if (!isComplete(header)) {
      size_t expandsize = MMAXLEN;
      char buffer[MMAXLEN] = {0};
      int size = Read(buffer, expandsize);
      if (-1 == size) {
        std::cout << "errno: " << errno << ", " << strerror(errno) << '\n';
        return false;
      }
      buffer_.assignBuffer(buffer, size);
      if (!isComplete(buffer_.getBuffer())) {
        throw std::runtime_error("http头读取不完整");
        return false;
      }
    }
    return true;
  }

  int fd_{-1};
  Buffer buffer_;
};


} // namespace httpserver

#endif // HTTP_SREVER_CONNECTION_HPP__

