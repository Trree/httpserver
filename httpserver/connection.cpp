
#include "connection.hpp"
#include "connection_manager.hpp"
#include <string>

namespace httpserver {

void Connection::start() {
  if (handleRead() && buffer_.isReady()) {
    Request req(buffer_.getBuffer());
    std::string response = req.handleRequest();
    handleWrite(response);
  }
  else {
    stop();
  }
}

void Connection::stop() 
{
  close(fd_);
  fd_ = -1;
  connections_manager_.stop(shared_from_this());
}

int Connection::getFd() 
{
  return fd_;
}

void Connection::setFd(int fd) 
{
  fd_ = fd;
}

int Connection::handleWrite(std::string response)
{
  std::cout << "Start write ..." << '\n';
  ssize_t wlen = 0;
  int size = response.size();
  const char *buffer = response.c_str();
  for (;;) {
    wlen = send(fd_, buffer, size, 0);
    if (wlen == 0) {
      return 0;
    }

    if (wlen == -1) {
      if (errno == EAGAIN) {
        break;
      }
      std::cout << "send: wlen: " << wlen << " errno: " <<  errno << " " << strerror(errno) << '\n';
      return -1;
    }
    std::cout << "send: fd:" << fd_ << ' ' << wlen << " of" << ' ' << size << '\n'; 
    buffer += wlen;
    size -= wlen;
  }

  return wlen;
}
 
bool Connection::handleRead()
{
  size_t len = MAXLEN;
  char buffer[MAXLEN] = {0};
  int rlen = Read(buffer, len);
  if (rlen <= 0) {
    stop();
    throw std::logic_error("read return zero. so close.");
  }
  if (!isComplete(buffer_.getBuffer())) {
    return false;
  }
  return true;
}

int Connection::Read(char* buffer, size_t size) 
{
  std::cout << "Start read ..." << '\n';
  ssize_t n = 0;
  for(;;) {
    n = recv(fd_, buffer, size, 0);

    if (n == 0) {
      return 0;
    }

    if (n > 0) {
      buffer_.assignBuffer(buffer, size);
      std::cout << "recv: fd:" << fd_ << ' ' << n << " of" << ' ' << size << '\n'; 
    }

    if (n == -1 && errno == EAGAIN) {
      if (errno == EAGAIN) {
        buffer_.setReady();
        break;
      }
      return -1;
    }
  } 

  auto len = buffer_.size();
  return (len !=  0) ? len : n; 
} 

bool Connection::isComplete(std::string header) {
  auto n = header.find("\r\n\r\n");
  if (n == std::string::npos) {
    return false;
  }
  return true;
}

} // namespace httpserver
