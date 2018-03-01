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
  Response(std::string requesturi, std::string rootdir) {
    std::string filepath = requesturi.append(rootdir);
    std::ifstream t(filepath);
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    body_ = str;
   
    response_.clear();
    response_.append("HTTP/1.1 200 OK\r\n");

    std::map<std::string, std::string> re;
    re.insert(std::pair<std::string, std::string>("Content-Length", std::to_string(body_.size())));
    re.insert(std::pair<std::string, std::string>("Content-Type", "text/html"));
    for (auto value : re) {
      response_.append(value.first);
      response_.append(": ");
      response_.append(value.second);
      response_.append("\r\n");
    }
    response_.append("\r\n");
  }

  Response(const Response&) = delete;
  Response& operator=(Response&) = delete;

  const std::string getResponse() {
    return response_.append(body_);
  }

private:
  std::string response_;
  std::string body_;
};


}// namespace httpserver

#endif // HTTP_SREVER_RESPONSE_HPP_ 
