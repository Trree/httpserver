#ifndef HTTP_SREVER_PARSEMIME_HPP_
#define HTTP_SREVER_PARSEMINE_HPP_

#include "handle_string.hpp"
#include <string>
#include <map>
#include <fstream>

namespace httpserver {

class ParseMime {
public:
  ParseMime(std::string filename) : filename_(filename) {}

  void parseMime() {
    std::ifstream input(filename_.c_str());
    for (std::string line; getline(input, line);) {
      std::string key;
      std::string value;
      try {
        std::tie(key, value) = parseline(line, ' ');
      }
      catch (const std::exception& e) {
        std::cout << e.what() << '\n';
        continue;
      }
      conf_.insert(std::pair<std::string, std::string>(value, key));
    }
  }

  std::map<std::string, std::string>& getConf() {
    return conf_;
  }
  void printConf() {
    for (auto p : conf_) {
      std::cout << p.first << ":" << p.second << '\n';
    }
  }


private:
  std::string filename_;
  std::map<std::string, std::string> conf_;  
};

} // namespace httpserver
#endif // HTTP_SREVER_PARSEMINE_HPP_
