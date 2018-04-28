#include <map>
#include <tuple>
#include <string>
#include <exception>
#include <iterator>

namespace httpserver {

std::string trim(std::string s)
{
  if (s.empty()){
    return s;
  }
  std::string::iterator npos;
  for (npos = s.begin(); npos != s.end();) {
    if (std::isspace(*npos)) {
      npos++;
    }
    else {
      break;
    }
  }
  s.erase(s.begin(), npos);

  std::string::reverse_iterator rnpos;
  for (rnpos = s.rbegin(); rnpos != s.rend();) {
    if (std::isspace(*rnpos)) {
      rnpos++;
    }
    else {
      break;
    }
  }

  s.erase(--rnpos.base());
  return s;
}

std::tuple<std::string, std::string> parseline(std::string line, char delimiter) 
{
  if (line.back() == '\r') {
    line = line.substr(0, line.size() - 1);
  }

  auto search = line.find_first_of(delimiter);
  if (search == std::string::npos) {
    throw std::invalid_argument(line.c_str());
  }
  auto key = line.substr(0, search);
  auto value = line.substr(search);
  if (key.empty() || value.empty()) {
    throw std::invalid_argument(line);
  }
  key = trim(key);
  value = trim(value);
  return std::make_tuple(key, value);
}

} // namespace httpserver
