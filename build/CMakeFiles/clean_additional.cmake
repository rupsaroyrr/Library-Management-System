# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "BookNest_autogen"
  [[CMakeFiles\BookNest_autogen.dir\AutogenUsed.txt]]
  [[CMakeFiles\BookNest_autogen.dir\ParseCache.txt]]
  [[CMakeFiles\test_database_autogen.dir\AutogenUsed.txt]]
  [[CMakeFiles\test_database_autogen.dir\ParseCache.txt]]
  "test_database_autogen"
  )
endif()
