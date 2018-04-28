
add_library(http_testtime SHARED
  ../handle_string.cpp 
)
target_sources(http_testtime PRIVATE
  trim_unittest.cc
)

add_executable(trim_unittest trim_unittest.cc)
#add_executable(benchmark test.cc)

target_link_libraries(trim_unittest http_testtime)

#add_test(NAME httpserver.benchmark COMMAND benchmark)
add_test(NAME httpserver.trim COMMAND trim_unittest)
