#include <map>
#include <tuple>
#include <string>
#include <exception>
#include <algorithm>

namespace httpserver {

static inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
    return !std::isspace(ch);
  }));
}

static inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
    return !std::isspace(ch);
  }).base(), s.end());
}

std::string trim(std::string s)
{
  ltrim(s);
  rtrim(s);
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
