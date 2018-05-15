#pragma once 

#include "connection_manager.hpp"
#include "socket.hpp"
#include "event.hpp"
#include <string>

namespace httpserver {
 
class HttpServer {
public:
  HttpServer(const HttpServer&) = delete;
  HttpServer& operator=(const HttpServer&) = delete;
  
  HttpServer(const std::string& ip, const std::string& port)
  : socket_(), event_(), connections_manager_(){
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

