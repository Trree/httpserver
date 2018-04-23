#include "../connection.hpp"
#include "response.hpp"
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace httpserver {

std::string Response::getBody(std::string path, std::string rootdir) {
  std::string filepath = path.append(rootdir);
  std::ifstream t(filepath);
  std::string str((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());
  return str;  
}

void Response::handleResponse(std::string path, std::string rootdir) {
  body_ = getBody(path, rootdir);
 
  response_.clear();
  response_.append("HTTP/1.1 200 OK\r\n");
  setResponseHeader("Content-Length", std::to_string(body_.size()));
  setResponseHeader("Content-Type", "text/html");
  if (connptr_->getKeepalive()) {
    setResponseHeader("Connection", "Keep-Alive");
  }
  response_ += header_to_string();
  //response_.append(body_);
  connptr_->handleWrite(response_);

  std::string fromfile= path.append(rootdir);
  connptr_->setFile(fromfile);
  connptr_->sendfile();
}


} // namespace httpserver
