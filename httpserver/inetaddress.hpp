#ifndef HTTP_SREVER_INETADDRESS_HPP_
#define HTTP_SREVER_INETADDRESS_HPP_ 

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string> 

namespace httpserver {

struct addrinfo* handleInetAddress(const std::string& addr, const std::string& port);

} // namespace httpserver
#endif // HTTP_SREVER_INETADDRESS_HPP_
