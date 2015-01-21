cmake_minimum_required(VERSION 2.6)
set(TARGET ircsq)
project(${TARGET})

find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads)

if(UNIX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
endif()

link_directories(
  SQUIRREL3/lib
  )
include_directories(${TARGET}
  SQUIRREL3/include
  )
add_executable(${TARGET}
  main.cc
  ircsq.cc
  irc/bot.cc
  )


target_link_libraries(${TARGET}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${CMAKE_THREAD_LIBS_INIT}
  squirrel
  sqstdlib
  )
