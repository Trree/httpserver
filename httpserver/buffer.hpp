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
  Buffer(Buffer&&) = default;
  Buffer& operator=(Buffer&) = delete;
  Buffer& operator=(Buffer&&) = default;
  

  std::string assignBuffer(char* buffer, int size) {
    if (size > 0) {
      std::string s(buffer, size);
      request_ += s;
    }
    return request_;
  }

  const std::string getBuffer() const {
    return request_;
  }

  size_t size() const{
    return request_.size();
  }

  bool isReady() const {
    return ready_;
  }

  void setReady() {
    ready_ = true;
  }

  ~Buffer() {}

private:
  std::string request_;
  bool ready_{false};
};

} // namespace httpserver

#endif // HTTP_SREVER_BUFFER_HPP_
