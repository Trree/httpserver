#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../httpserver/handle_string.hpp"
#include <string>

using namespace httpserver;
TEST_CASE("testing the trim function") {
  std::string base("aa");
  CHECK(trim("aa") == base);
  CHECK(trim(" aa") == base);
  CHECK(trim("aa ") == base);
  CHECK(trim(" aa ") == base);
  CHECK(trim("     aa   ") == base);
  CHECK(trim("        ") == "");
  CHECK(trim("") == "");
}
