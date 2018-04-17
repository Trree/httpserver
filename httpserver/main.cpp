/*************************************************************************
	> File Name: server.cpp
	> Author: 
	> Mail: 
	> Created Time: Fri Feb  9 23:47:02 2018
 ************************************************************************/
#include "server.hpp"

using namespace httpserver;

int main()
{
  HttpServer server("::", "9999");
  server.handleEvent();

  return 0;
}
