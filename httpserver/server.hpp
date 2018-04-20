#ifndef HTTP_SREVER_SERVER_HPP_
#define HTTP_SREVER_SERVER_HPP_ 

#include "connection_manager.hpp"
#include "socket.hpp"
#include "event.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h> 
#include <exception>

namespace httpserver {
 
using connection_ptr = std::shared_ptr<Connection>;

class HttpServer {
public:
  HttpServer(const HttpServer&) = delete;
  HttpServer& operator=(const HttpServer&) = delete;
  
  explicit HttpServer(const std::string& ip = "::", 
                      const std::string& port = "9999")
  : socket_(ip, port), event_(){
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
