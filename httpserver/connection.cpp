
#include "connection.hpp"
#include "connection_manager.hpp"
#include <string>
#include <sys/types.h>
#include <sys/socket.h>

namespace httpserver {

void Connection::start() {
  if (handleRead() && buffer_.isReady()) {
    Request req(shared_from_this(), buffer_.getBuffer());
    std::string response = req.handleRequest();
    setStatus(StatusType::closed);
    if (!getKeepalive()) {
      stop();
    }
  }
}

void Connection::stop() 
{
  connections_manager_.stop(getKey());
}

int Connection::getKey()
{
  return key_;
}

int Connection::Read(char* buffer, size_t size) 
{
  std::cout << "Start read ..." << '\n';
  ssize_t n = 0;
  for(;;) {
    n = recv(socket_.getfd(), buffer, size, 0);

    if (n == 0) {
      return 0;
    }

    if (n > 0) {
      buffer_.assignBuffer(buffer, size);
      std::cout << "recv: fd:" << socket_.getfd() << ' ' << "flag: " << key_ << ' '<< n << " of" << ' ' << size << '\n'; 
    }

    if (n == -1 && errno == EAGAIN) {
      if (errno == EAGAIN) {
        buffer_.setReady();
        break;
      }
      std::cout << "read: rlen: " << n << " errno: " <<  errno << " " << strerror(errno) << '\n';
      return -1;
    }
  } 

  auto len = buffer_.size();
  return (len !=  0) ? len : n; 
} 
 
bool Connection::handleRead()
{
  size_t len = MAXLEN;
  char buffer[MAXLEN] = {0};
  int rlen = Read(buffer, len);
  if (rlen <= 0) {
    stop();
    return false;
  }
  if (!isComplete(buffer_.getBuffer())) {
    return false;
  }
  setStatus(StatusType::read);
  end_time_ = std::chrono::high_resolution_clock::now();
  return true;
}

bool Connection::isComplete(std::string header) {
  auto n = header.find("\r\n\r\n");
  if (n == std::string::npos) {
    return false;
  }
  return true;
}

int Connection::handleWrite(std::string response)
{
  std::cout << "Start write ..." << '\n';
  ssize_t wlen = 0;
  int size = response.size();
  const char *buffer = response.c_str();


  for (;;) {
    wlen = send(socket_.getfd(), buffer, size, 0);
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
    std::cout << "send: fd:" << socket_.getfd() << ' ' << wlen << " of" << ' ' << size << '\n'; 
    buffer += wlen;
    size -= wlen;
  }

  setStatus(StatusType::write);
  end_time_ = std::chrono::high_resolution_clock::now();
  return wlen;
}

} // namespace httpserver
