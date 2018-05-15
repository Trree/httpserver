#pragma once 

#include <exception>

namespace httpserver{

struct ExceptionBase : virtual std::exception {};
struct FileOpenErr : virtual ExceptionBase{};

} // namespace httpserver 
