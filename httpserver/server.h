#include <sys/types.h>
#include <sys/socket.h>

class HttpServer {
public:
  HttpServer(const std::string& ip, const std::string& port, const std::string rootdir) : ip_(ip), port_(port), rootdir_(rootdir) {
    file_ = socket(ip, SOCK_STREAM, AF_INET);
    const struct sockaddr addr;
    memset(addr, sizeof(addr), 0);

    auto ret = bind(file_, )
  }
 
  
  
private:
  File file_;

  std::string ip_("0.0.0.0");
  std::string port_("80");
  std::string rootdir_("/var/www");
}


       int bind(int sockfd, const struct sockaddr *addr,
                               socklen_t addrlen);

