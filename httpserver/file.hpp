#pragma once 

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <string>
#include <iostream>
#include <queue>

namespace httpserver {

class File {
public:
  File(const File&) = delete;
  File& operator=(const File&) = delete;

  File() : fd_(-1) {}
  File(int fd) : fd_(fd) {}
  File(std::string filename) 
  : name_(filename), isopen_(false), offset_(0) 
  {
    fd_ = open(filename.c_str(), O_RDONLY | O_NONBLOCK);
    isopen_ = true;
    struct stat st;
    stat(filename.c_str() ,&st);
    filelen_ = st.st_size;
  }

  void swap(File& lhr, File& rhr) {
    using std::swap;
    std::swap(lhr.fd_, rhr.fd_);
    std::swap(lhr.name_, rhr.name_);
    std::swap(lhr.isopen_, rhr.isopen_);
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
    if (fd_ == -1) {
      std::cout << "Failed file descriptor is -1" << '\n';
      return 0;
    }
    while (offset_ < filelen_) {
      sendlen = ::sendfile(fd, fd_, &offset_, filelen_);
      std::cout << fd << ":sendfile " << fd_ << " : offset is " << offset_ << '\n'; 
      if (sendlen < 0) {
        break;
      }
    }
    return sendlen;
  }

  bool finish() {
    int rest = filelen_ - offset_;
    if(rest) {
      return false;
    }
    return true;
  }

  int close() {
    int ret = 0;
    if (isopen_ && fd_ != -1) {
      ret = ::close(fd_);
      std::cout << "close filename " << name_ <<" : " << fd_ << '\n'; 
    }
    return ret;
  }

  ~File() {
    close();
  }

private:
  int fd_{-1};
  std::string name_;
  bool isopen_;
  off_t offset_;
  int filelen_;
};

class FileChain{
public:
  FileChain() {}
  void push(File&& file) {
    fs_.push(std::move(file));
  }
  void pop(int fd) {
    fs_.front().sendfile(fd);
    if (fs_.front().finish()) {
      fs_.pop();
    }
  }
  bool empty() {
    return fs_.empty();
  }

private:
  std::queue<File> fs_;
};
}// namespace httpserver

