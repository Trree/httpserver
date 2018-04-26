#include "inetaddress.hpp"
#include <string.h>

namespace httpserver {

struct addrinfo* handleInetAddress(const std::string& addr, const std::string& port) {
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
