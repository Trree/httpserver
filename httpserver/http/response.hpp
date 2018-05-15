#pragma once 

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <fstream>
#include <iterator>
#include <memory>

namespace httpserver {

class Connection;

class Response{
public:
  Response(const Response&) = delete;
  Response& operator=(const Response&) = delete;
  
  Response(std::shared_ptr<Connection> connptr) : connptr_(connptr), response_(), re_(), body_(){}
  void handleResponse(std::string path, std::string rootdir);
  std::string getBody(std::string path, std::string rootdir);

private:

  void setResponseHeader(std::string key, std::string value) {
    re_.insert(std::pair<std::string, std::string>(key, value));
  }

  std::string header_to_string(){
    std::string response;
     for (auto value : re_) {
      response.append(value.first);
      response.append(": ");
      response.append(value.second);
      response.append("\r\n");
    }
    response.append("\r\n");
    return response;
  }


  std::shared_ptr<Connection> connptr_;
  std::string response_;
  std::map<std::string, std::string> re_;
  std::string body_;
};

}// namespace httpserver
