#ifndef HTTP_SREVER_BUFFER_HPP_
#define HTTP_SREVER_BUFFER_HPP_

#include <string.h>
#include <string>

namespace httpserver {

class Buffer {
public:
  Buffer() {}

  Buffer(char* buffer, int size) {
    request_.assign(buffer, size);
  }

  Buffer(const Buffer&) = delete;
  Buffer& operator=(Buffer&) = delete;

  std::string assignBuffer(char* buffer, int size) {
    std::string s(buffer, size);
    request_ += s;
    return request_;
  }

  std::string getBuffer(){
    return request_;
  }

  bool isReady() const {
    return ready_;
  }

  void setReady() {
    ready_ = 1;
  }

  ~Buffer() {}

private:
  std::string request_;
  bool ready_ = false;
};

} // namespace httpserver

#endif // HTTP_SREVER_BUFFER_HPP_
