#ifndef HTTP_SERVER_EXCEPTION_BASE_HPP_
#define HTTP_SERVER_EXCEPTION_BASE_HPP_

#include <exception>

namespace httpserver{
struct ExceptionBase : virtual std::exception {};


} // namespace httpserver 

#endif // HTTP_SERVER_EXCEPTION_BASE_HPP_
