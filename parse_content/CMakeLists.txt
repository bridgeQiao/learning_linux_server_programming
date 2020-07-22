cmake_minimum_required(VERSION 3.5)

project(parse_content LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(parse_content main.cc parse_content.cc)
