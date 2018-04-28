#ifndef HTTP_SREVER_SERVER_HPP_
#define HTTP_SREVER_SERVER_HPP_ 

#include "connection_manager.hpp"
#include "socket.hpp"
#include "event.hpp"
#include <string>

namespace httpserver {
 
class HttpServer {
public:
  HttpServer(const HttpServer&) = delete;
  HttpServer& operator=(const HttpServer&) = delete;
  
  explicit HttpServer(const std::string& ip, const std::string& port)
  : event_(){
    socket_.bindAddress(ip, port);
    socket_.listen();
    event_.add(socket_.getfd(), 0, EPOLLIN);
  }
  ~HttpServer() {}

  void handleEvent();
private:
  Socket handleAccept();
  
  Socket socket_;
  Event event_;
  ConnectionManager connections_manager_;
};

} //namespace httpserver

#endif // HTTP_SREVER_SERVER_HPP_
