#ifndef HTTP_SREVER_PARSE_URI_HPP_
#define HTTP_SREVER_PARSE_URI_HPP_

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <iostream>
#include <map>

namespace httpserver {

class ParseUri {
public:
  ParseUri(const ParseUri&) = delete;
  ParseUri& operator=(const ParseUri&) = delete;
  
  ParseUri() {}
  explicit ParseUri(std::string& request) : method_("GET"), requesturi_("/index.html"), version_("HTTP/1.1") {
    auto n = request.find("\r\n");
    std::string requestline = request.substr(0, n);
    handleRequestUri(requestline);
    auto headlen = request.find("\r\n\r\n");
    std::cout << requestline << '\n';
    std::string header = request.substr(n, headlen);
    getRequestHeader(header);
    printHeader();
    body_ = request.substr(headlen + 3);
    request.clear();
  }

  const std::string getMethod() const {
    return method_;
  }

  const std::string getRequestUri() const {
    return requesturi_;
  }

  const std::string getVersion() const {
    return version_;
  }

  const std::map<std::string, std::string> getHeader() const {
    return reqheader_;
  }

  bool isKeepalive() {
    auto search = reqheader_.find("Connection");
    if (search == reqheader_.end()) {
      return false;
    }
    std::string data = reqheader_.at("Connection");
    std::transform(data.begin(), data.end(), data.begin(), ::tolower);
    if (data == "keep-alive") {
      return true;
    }
    return false;
  }

private:

  std::string& trim(std::string &s)
  {
    if (s.empty()){
      return s;
    }
    s.erase(0,s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
  }

  void getRequestHeader(std::string& header) {
    std::istringstream iss(header);
    std::vector<std::string> tokens;
    std::string key;
    std::string value;
    std::string item;
    while (std::getline(iss, item)) {
      if (item.back() == '\r') {
        item = item.substr(0, item.size() - 1);
      }
      auto n = item.find(':');
      if (n == std::string::npos) {
        continue;
      }
      key = item.substr(0, n);
      value = item.substr(n+1);
      if (key.empty() || value.empty()) {
        continue;
      }
      key = trim(key);
      value = trim(value);
      reqheader_.insert(std::pair<std::string, std::string>(key, value));
    }
  }

  void printHeader() {
    for (auto p : reqheader_) {
      std::cout << p.first << ":" << p.second << '\n';
    }
  }

  void handleRequestUri(std::string &requestline) {
    std::istringstream iss(requestline);
    std::vector<std::string> tokens;
    std::copy(std::istream_iterator<std::string>(iss),
              std::istream_iterator<std::string>(),
              std::back_inserter(tokens));
    method_ = tokens[0];
    requesturi_ = tokens[1];
    version_ = tokens[2];
  }

  std::string method_;
  std::string requesturi_;
  std::string version_;
  std::map<std::string, std::string> reqheader_;
  std::string body_;
  std::vector<std::map<std::string, std::string>> headers_;
};

} // namespace httpserver
#endif // HTTP_SREVER_PARSE_URI_HPP__
