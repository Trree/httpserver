#ifndef HTTP_SREVER_CONNECTION_MANAGER_HPP_
#define HTTP_SREVER_CONNECTION_MANAGER_HPP_

#include "connection.hpp"
#include <memory>
#include <set>

namespace httpserver {

using connection_ptr = std::shared_ptr<Connection>;

class ConnectionManager {
public:
  ConnectionManager(const ConnectionManager&) = delete;
  ConnectionManager& operator=(const ConnectionManager&) = delete;
  
  ConnectionManager() {}
  ConnectionManager(connection_ptr c) {
    connections_.insert(c);
  }
  ~ConnectionManager(){}
  void start(connection_ptr conn) {
    connections_.insert(conn);
  }

  void stop(connection_ptr c) {
    connections_.erase(c);
  }

  void stop_all() {
    connections_.clear();
  }
private:
  std::set<connection_ptr> connections_;
};

} // namespace httpserver
#endif // HTTP_SREVER_CONNECTION_MANAGER_HPP__
