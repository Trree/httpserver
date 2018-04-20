#ifndef HTTP_SREVER_REQUEST_HPP_
#define HTTP_SREVER_REQUEST_HPP_

#include <string>
#include "response.hpp"
#include "parse_uri.hpp"
#include <memory>

namespace httpserver{

class Connection;

class Request {
public:
  Request(const Request&) = delete;
  Request& operator=(const Request) = delete;

  Request(std::shared_ptr<Connection> connptr, std::string& header) 
  : connptr_(connptr),  parseuri_(header), resp_(connptr){}

  std::string handleRequest(); 
private:
  std::shared_ptr<Connection> connptr_;
  ParseUri parseuri_;
  Response resp_;
};

} // namespace httpserver

#endif // HTTP_SREVER_REQUEST_HPP_
