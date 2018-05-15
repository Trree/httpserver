#pragma once 

#include "handle_string.hpp"
#include <string>
#include <map>
#include <fstream>

namespace httpserver {

class ParseConf {
public:
  ParseConf(std::string filename) : filename_(filename) {}

  void parseConf() {
    std::ifstream input(filename_.c_str());
    for (std::string line; getline(input, line);) {
      line = trim(line);
      if (line.front() == '#') {
        continue;
      }

      std::string key;
      std::string value;
      try {
        std::tie(key, value) = parseline(line, ' ');
      }
      catch (const std::exception& e) {
        std::cout << e.what() << '\n';
        continue;
      }
      conf_.insert(std::pair<std::string, std::string>(key, value));
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

  std::string get(std::string key) {
    auto search = conf_.find(key);
    if (search != conf_.end()) {
      return search->second;
    }
    return std::string();
  }

private:
  std::string filename_;
  std::map<std::string, std::string> conf_;  
};

} // namespace httpserver
