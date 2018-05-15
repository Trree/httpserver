#pragma once 

#include <string>
#include <tuple>

namespace httpserver {

std::string trim(std::string s);
std::tuple<std::string, std::string> parseline(std::string line, char delimiter);
}

