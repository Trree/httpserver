#ifndef HTTP_SREVER_FILE_HPP_
#define HTTP_SREVER_FILE_HPP_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <string>
#include <iostream>

namespace httpserver {

class File {
public:
  File(const File&) = delete;
  File& operator=(const File&) = delete;

  File() : fd_(-1) {}
  explicit File(std::string filename) 
  : name_(filename), offset_(0) 
  {
    fd_ = open(filename.c_str(), O_RDONLY);
    struct stat st;
    stat(filename.c_str() ,&st);
    filelen_ = st.st_size;
  }
  explicit File(int fd) : fd_(fd) {}

  void swap(File& lhr, File& rhr) {
    using std::swap;
    std::swap(lhr.fd_, rhr.fd_);
    std::swap(lhr.name_, rhr.name_);
    std::swap(lhr.offset_, rhr.offset_);
    std::swap(lhr.filelen_, rhr.filelen_);
  }
 
  File(File&& other) {
    swap(*this, other);
  }
  File& operator=(File&& other) {
    swap(*this, other);
    return *this;
  }

  int sendfile(int fd) {
    int sendlen = 0;
    while (offset_ < filelen_) {
      int sendlen = ::sendfile(fd, fd_, &offset_, 2048);
      if (sendlen < 0) {
        break;
      }
    }
    return sendlen;
  }

  int close() {
    int ret = ::close(fd_);
    return ret;
  }

  ~File() {
    std::cout << "close filename " << name_ <<" : " << fd_ << '\n'; 
    close();
  }

private:
  int fd_;
  std::string name_;
  off_t offset_;
  int filelen_;
};

}// namespace httpserver

#endif // HTTP_SREVER_FILE_HPP_
