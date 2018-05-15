/*************************************************************************
	> File Name: server.cpp
	> Author: 
	> Mail: 
	> Created Time: Fri Feb  9 23:47:02 2018
 ************************************************************************/
#include "server.hpp"
#include "parseconf.hpp"
#include "inetaddress.hpp"

using namespace httpserver;

int main(int argc, char* argv[]) 
{
  int opt;
  std::string filename("../conf/http.conf");
  std::string host("::");
  std::string port("9999");
  while ((opt = getopt(argc, argv, "hc:")) != EOF) {
    switch (opt) {
    case 'h':
      fprintf(stderr, "Usage: %s [-c filename]\n", argv[0]);
      exit(1);
    case 'c':
      filename = optarg;
      break;
    default:
      fprintf(stderr, "Usage: %s [-c filename]\n", argv[0]);
      exit(1);
    }
  }

  ParseConf conf(filename);
  conf.parseConf();
  auto addr = conf.get("listen");
  if (!addr.empty()) {
    std::tie(host, port) = parseurl(addr);
  }
  conf.printConf();
  HttpServer server(host, port);
  server.handleEvent();

  return 0;
}
