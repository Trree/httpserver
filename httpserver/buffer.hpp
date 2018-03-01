#ifndef HTTP_SREVER_BUFFER_HPP_
#define HTTP_SREVER_BUFFER_HPP_

#include <string.h>
#include <string>

namespace httpserver {

class Buffer {
public:
  Buffer() : request_(""), ready_(false)  {
    buffer_ = new char[1024];
    memset(buffer_, 0, 1024);
  }

  char* getBuffer() {
    return buffer_;
  }

  std::string getRequest() {
    return request_;
  }

  bool isReady() const {
    return ready_;
  }

  bool isComplete() {
    char *p = buffer_;
    while (*p != '\0') {
      if (*p++ == '\r') {
        if (strncmp(p, "\n\r\n", 3) == 0) {
          return true;
        }
      }
    }
    return false;
  }

  void setReady() {
    ready_ = 1;
  }

  ~Buffer() {
    if (buffer_) {
      delete [] buffer_;
    }
  }

private:
  char *buffer_;
  std::string request_;
  bool ready_;
};

} // namespace httpserver

#endif // HTTP_SREVER_BUFFER_HPP_
