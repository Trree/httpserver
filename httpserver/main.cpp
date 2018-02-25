/*************************************************************************
	> File Name: server.cpp
	> Author: 
	> Mail: 
	> Created Time: Fri Feb  9 23:47:02 2018
 ************************************************************************/
#include "server.hpp"
#include<iostream>

using namespace httpserver;

int main()
{
  HttpServer server("0.0.0.0", "9999");
  server.handleEvent();

  return 0;
}
