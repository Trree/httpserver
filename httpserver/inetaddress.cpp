#include "inetaddress.hpp"
#include "handle_string.hpp"
#include <string.h>
#include <iostream>
#include <tuple>
#include <string>

namespace httpserver {

std::tuple<std::string, std::string> parseurl(const std::string& url)
{
  std::string host;
  std::string port;
  auto search = url.find_last_of(':');
  if (search == std::string::npos) {
    host = "::";
    port = url;
  }
  else {
    host = url.substr(0, search);
    port = url.substr(search+1);
  }
  if (host.front() == '[') {
    host = host.substr(1, host.size() - 2);
  }
  return std::make_tuple(host, port);
}

struct addrinfo* handleInetAddress(const std::string& addr, const std::string& port)
{
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
    
  struct addrinfo *result;
  int ret = getaddrinfo(addr.c_str(), port.c_str(),&hints, &result);
  if (ret != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
    exit(EXIT_FAILURE);
  }
  return result;
}
} // namespace httpserver 
