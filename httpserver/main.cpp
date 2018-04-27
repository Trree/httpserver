/*************************************************************************
	> File Name: server.cpp
	> Author: 
	> Mail: 
	> Created Time: Fri Feb  9 23:47:02 2018
 ************************************************************************/
#include "server.hpp"
#include "parseconf.hpp"

using namespace httpserver;

int main()
{
  ParseConf conf("/home/trree/workspace/github/httpserver/httpserver/conf/http.conf");
  conf.parseConf();
  conf.printConf();
  HttpServer server("::", "9999");
  server.handleEvent();

  return 0;
}
