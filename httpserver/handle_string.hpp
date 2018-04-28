#ifndef HTTP_SREVER_HANDLE_STRING_HPP_
#define HTTP_SREVER_HANDLE_STRING_HPP_

namespace httpserver {

std::string& trim(std::string &s);
std::tuple<std::string, std::string> parseline(std::string line, char delimiter);
}

#endif // HTTP_SREVER_HANDLE_STRING_HPP_

