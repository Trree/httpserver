#ifndef HTTP_SREVER_FILE_HPP_
#define HTTP_SREVER_FILE_HPP_

namespace httpserver {

class File {
  File(int fd) : fd_(fd) {}
  File(const File&) = delete;
  File(File&& other) : fd_(std::move(other.fd_)) {}

private:
  int fd_{-1};
}
} // namespace httpserver

#endif // HTTP_SREVER_FILE_HPP_
