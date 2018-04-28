#include <map>
#include <tuple>
#include <string>
#include <exception>

namespace httpserver {

std::string& trim(std::string &s)
{
  if (s.empty()){
    return s;
  }
  s.erase(0,s.find_first_not_of(" "));
  s.erase(s.find_last_not_of(" ") + 1);
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
