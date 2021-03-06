#pragma once 

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string> 

namespace httpserver {

struct AddrinfoGuard {
  AddrinfoGuard(struct addrinfo* addrinfo) : info_(addrinfo) {}
  ~AddrinfoGuard() {
    freeaddrinfo(info_);
  }

  struct addrinfo *info_;
};

struct addrinfo* handleInetAddress(const std::string& addr, const std::string& port);
std::tuple<std::string, std::string> parseurl(const std::string& url);

} // namespace httpserver
