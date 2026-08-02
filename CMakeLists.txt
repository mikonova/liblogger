project(liblogger)
cmake_minimum_required(VERSION 4.4)

add_library(logger SHARED src/liblogger.cpp src/liblogger.hpp)
set_target_properties(logger PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_library(loggerstatic STATIC src/liblogger.cpp src/liblogger.hpp)
set_target_properties(loggerstatic PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_executable(loggertest src/loggertest.cpp)
set_target_properties(loggertest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/cli)

target_link_libraries(loggertest PRIVATE loggerstatic)