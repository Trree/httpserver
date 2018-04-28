#add_executable(unittest test.cc)

add_library(http_testtime SHARED
  ../handle_string.cpp 
)
target_sources(http_testtime PRIVATE
  trim.cc
)

add_executable(trim trim.cc)
#add_executable(benchmark test.cc)

target_link_libraries(trim http_testtime)

#add_test(NAME httpserver.benchmark COMMAND benchmark)
add_test(NAME httpserver.trim COMMAND trim)
