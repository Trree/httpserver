#ifndef HTTP_SREVER_HTTP_HANDLEMESSAGE_HPP_
#define HTTP_SREVER_HTTP_HANDLEMESSAGE_HPP_

#include <memory>
#include <string>

namespace httpserver{

class Connection;
bool isComplete(std::string buffer);
void handleMessage(std::shared_ptr<Connection> connptr, std::string& buffer);

} // namespace httpserver

#endif // HTTP_SREVER_HTTP_HANDLEMESSAGE_HPP_
