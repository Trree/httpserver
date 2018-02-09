# add the executable
add_executable(unittest test.cc)

add_test(NAME IndefiniteStream.unittest COMMAND unittest)
