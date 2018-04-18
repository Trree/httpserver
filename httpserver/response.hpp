#ifndef HTTP_SREVER_RESPONSE_HPP_
#define HTTP_SREVER_RESPONSE_HPP_

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <fstream>
#include <iterator>

namespace httpserver {

class Response{
public:
  Response(const Response&) = delete;
  Response& operator=(const Response&) = delete;
  
  Response() {}
  std::string handleResponse(std::string path, std::string rootdir) {
    body_ = getBody(path, rootdir);
   
    response_.clear();
    response_.append("HTTP/1.1 200 OK\r\n");
    setResponseHeader("Content-Length", std::to_string(body_.size()));
    setResponseHeader("Content-Type", "text/html");
    response_ += header_to_string();
    return response_.append(body_);
  }

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

  std::string getBody(std::string path, std::string rootdir) {
    std::string filepath = path.append(rootdir);
    std::ifstream t(filepath);
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    return str;  
  }

  std::string response_;
  std::map<std::string, std::string> re_;
  std::string body_;
};


}// namespace httpserver

#endif // HTTP_SREVER_RESPONSE_HPP_ 
