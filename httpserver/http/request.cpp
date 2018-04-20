#include "../connection.hpp"
#include "request.hpp"

namespace httpserver {

std::string Request::handleRequest() {
  if (parseuri_.isKeepalive()) {
    connptr_->setKeepalive(true);
  }
  std::string response;
  std::string rootdir("/var/www/html");
  resp_.handleResponse(rootdir, parseuri_.getRequestUri());
  return response; 
}

}
