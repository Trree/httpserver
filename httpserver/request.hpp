#ifndef HTTP_SREVER_REQUEST_HPP_
#define HTTP_SREVER_REQUEST_HPP_

#include <string>
#include "response.hpp"
#include "parse_uri.hpp"

namespace httpserver{

class Request {
public:
  Request(std::string header) : parseuri_(header), resp_(){}
  std::string handleRequest() {
    std::string response;
    std::string rootdir("/var/www/html");
    response = resp_.handleResponse(rootdir, parseuri_.getRequestUri());
    return response; 
  }

private:
 ParseUri parseuri_;
 Response resp_;
};

} // namespace httpserver

#endif // HTTP_SREVER_REQUEST_HPP_
