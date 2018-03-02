#ifndef HTTP_SREVER_CONNECTION_HPP_
#define HTTP_SREVER_CONNECTION_HPP_

#include "response.hpp"
#include "parse_uri.hpp"
#include "buffer.hpp"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>

#define MAXLEN 10
#define MMAXLEN 8192

namespace httpserver {

class Connection {
public:
  Connection(int fd) : fd_(fd), buffer_(MAXLEN) {}
  Connection(const Connection&) = delete;
  Connection& operator=(Connection&) = delete;
  ~Connection() {
    close(fd_);
    fd_ = -1;
  }
  
  bool handleConnection() {
    size_t size = MAXLEN;
    int len = Read(buffer_.getBuffer(), size);
    buffer_.setReadlen(len);
    if (checkComplete()) {
      std::string response = handleResponse();
      handleWrite(response);
    }
    else {
      return false;
    }
    return true;
  }
  
  int Read(char* buffer, size_t size) {
    ssize_t n = 0;
    do {
      n = recv(fd_, buffer, size, 0);

      if (n == 0) {
        return 0;
      }

      if (n > 0) {
        if ((size_t) n < size) {
          buffer_.setReady();
          return n;
        }
      }
    } while (errno == EINTR);

    return n;
  }

  bool checkComplete() {
    if (!buffer_.isComplete()) {
      size_t expandsize = MMAXLEN;
      Read(buffer_.getBuffer() +  buffer_.getReadlen(),expandsize);
      if (!buffer_.isComplete()) {
        return false;
      }
    }
    return true;
  }

  int handleWrite(std::string response) {
    size_t wlen = 0;
    int left = response.size();
    wlen = send(fd_, response.c_str(), left, 0);

    return wlen;
  }

  std::string handleResponse() {
    std::string response;
    ParseUri parseuri(buffer_.getBuffer());
    std::string rootdir("/var/www/html");
    Response re(rootdir, parseuri.getRequestUri());
    response = re.getResponse();
    return response; 
  }

private:
  int fd_ = -1;
  Buffer buffer_;
};


} // namespace httpserver

#endif // HTTP_SREVER_CONNECTION_HPP__

