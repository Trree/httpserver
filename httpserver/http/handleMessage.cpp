#include "../connection.hpp"
#include "response.hpp"
#include "parse_uri.hpp"
#include <memory>
#include <string>

namespace httpserver{

void handleMessage(std::shared_ptr<Connection> connptr, std::string& buffer) {
  ParseUri parseuri(buffer);
  Response response(connptr);
  if (parseuri.isKeepalive()) {
    connptr->setKeepalive(true);
  }
  std::string rootdir("/var/www/html");
  response.handleResponse(rootdir, parseuri.getRequestUri());
}

} // namespace httpserver

