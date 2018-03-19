#ifndef HTTP_SREVER_FILE_HPP_
#define HTTP_SREVER_FILE_HPP_

#include "exception_base.hpp"
#include <iostream>

namespace httpserver {

class File {
  File(int fd) : fd_(fd) {}
  File(const char* filename) : filehandle_(open(filename, "r")) {
    if (filehandle_ == -1) {
      throw FileOpenErr("fopen failed");
    }
  }
  ~File() {
    close(filehandle_);
  }

  int getFd() {
    return filehandle_;
  }
  File(const File&) = delete;
  File& operator=(const File&) = delete;

private:
  int filehandle_;
};

} // namespace httpserver

#endif // HTTP_SREVER_FILE_HPP_
