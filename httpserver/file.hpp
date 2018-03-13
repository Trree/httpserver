#ifndef HTTP_SREVER_FILE_HPP_
#define HTTP_SREVER_FILE_HPP_

#include <iostream>

namespace httpserver {

class File {
  File(int fd) : fd_(fd) {}
  File(const char* filename) : filehandle_(std::fopen(filename, "r")) {
    if (filehandle_ == NULL) {
      ;
    }
  }
  ~File() {
    std::fclose(filehandle_);
  }
  File(const File&) = delete;
  File& operator=(File&) = delete;

private:
  std::FILE* filehandle_;
};

} // namespace httpserver

#endif // HTTP_SREVER_FILE_HPP_
