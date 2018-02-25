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
  explicit ParseUri(char* buffer) : method_("GET"), requesturi_("/index.html"), version_("HTTP/1.1") {
    char *p = buffer;
    std::string requestline;
    while (*p != '\n') {
      requestline.push_back(*p);
      *p++;
    }
    handleRequestUri(requestline);
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

private:

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
  std::vector<std::map<std::string, std::string>> headers_;
};

} // namespace httpserver
#endif // HTTP_SREVER_PARSE_URI_HPP__
