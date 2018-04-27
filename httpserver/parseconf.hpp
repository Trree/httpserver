#ifndef HTTP_SREVER_PARSECONF_HPP_
#define HTTP_SREVER_PARSECONF_HPP_

#include <string>
#include <map>
#include <fstream>

namespace httpserver {

class ParseConf {
public:
  ParseConf(std::string filename) : filename_(filename) {}

  std::string& trim(std::string &s)
  {
    if (s.empty()){
      return s;
    }
    s.erase(0,s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
  }
  void parseConf() {
    std::ifstream input(filename_.c_str());
    for (std::string line; getline(input, line);) {
      auto search = line.find_first_of(' ');
      if (search == std::string::npos) {
        continue;
      }
      auto key = line.substr(0, search);
      auto value = line.substr(search);
      if (key.empty() || value.empty()) {
        continue;
      }
      key = trim(key);
      value = trim(value);
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


private:
  std::string filename_;
  std::map<std::string, std::string> conf_;  
};

} // namespace httpserver
#endif // HTTP_SREVER_PARSECONF_HPP_

