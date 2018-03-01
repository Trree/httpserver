#ifndef HTTP_SREVER_REQUEST_HPP_
#define HTTP_SREVER_REQUEST_HPP_

#include "response.hpp"
#include "parse_uri.hpp"
#include "buffer.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <string>

namespace httpserver{

class Request {
public:
  Request(int fd) : fd_(fd){}
  ~Request() {}

  int handleRead() {
    size_t size = 1024;
    ssize_t n = 0;
    do {
      n = recv(fd_, buffer_.getBuffer(), size, 0);

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

  int handleWrite(std::string response) {
    size_t wlen = 0;
    int left = response.size();
    wlen = send(fd_, response.c_str(), left, 0);

    return wlen;
  }

  std::string handleResponse() {
     std::string response;
    bool b = buffer_.isComplete();
    if (b) {
      ParseUri parseuri(buffer_.getBuffer());
      std::string rootdir("/var/www/html");
      Response re(rootdir, parseuri.getRequestUri());
      response = re.getResponse();
    }
    else {
      response = "data is not complete";
    }
    return response; 
  }

private:
  int fd_;
  Buffer buffer_;
};

} // namespace httpserver

#endif // HTTP_SREVER_REQUEST_HPP_
