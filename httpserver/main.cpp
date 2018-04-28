/*************************************************************************
	> File Name: server.cpp
	> Author: 
	> Mail: 
	> Created Time: Fri Feb  9 23:47:02 2018
 ************************************************************************/
#include "server.hpp"
#include "parseconf.hpp"

using namespace httpserver;

int main(int argc, char* argv[]) 
{
  int opt;
  std::string filename;
  while ((opt = getopt(argc, argv, "hc:")) != -1) {
    switch (opt) {
    case 'h':
      fprintf(stderr, "Usage: %s [-c filename]\n", argv[0]);
      exit(EXIT_SUCCESS);
    case 'c':
      filename = optarg;
      break;
    default:
      fprintf(stderr, "Usage: %s [-c filename]\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  if (filename.empty()) {
    filename = "../conf/http.conf";
  }
  ParseConf conf(filename);
  conf.parseConf();
  auto addr = conf.get("listen");
  std::string host;
  std::string port;
  if (addr.empty()) {
    host = "::";
    port = "9999";
  }
  else {
    std::tie(host, port) = parseurl(addr);
  }
  conf.printConf();
  HttpServer server(host, port);
  server.handleEvent();

  return 0;
}
