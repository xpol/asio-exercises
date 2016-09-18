cmake_minimum_required(VERSION 3.0)

project(asio-exercises CXX)

find_package(Boost REQUIRED COMPONENTS system date_time)
add_compile_options(-std=c++11)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

if(MSVC)
  add_definitions(-D_WIN32_WINNT=0x0501 -D_CRT_SECURE_NO_WARNINGS)
endif()

function(add_exercises name)
  cmake_parse_arguments(add_exercise "" "" "SOURCE" ${ARGN})
  if ("${add_exercise_SOURCE}" STREQUAL "")
    set(add_exercise_SOURCE src/${name}.cpp)
  endif()

  add_executable(${name} ${add_exercise_SOURCE})
  source_group(src FILES ${add_exercise_SOURCE})
  if (NOT MSVC) # MSVC version will use boost auto linking
      target_link_libraries(${name} ${Boost_LIBRARIES})
  endif()
endfunction()

add_exercises(timer-sync)
add_exercises(timer-async)
add_exercises(handler-args)
add_exercises(handler-memfn)
add_exercises(handler-thread-sync)
add_exercises(tcp-daytime-client-sync)
add_exercises(tcp-daytime-server-sync SOURCE src/tcp-daytime-server-sync.cpp src/daytime.hpp)
add_exercises(tcp-daytime-server-async SOURCE src/tcp-daytime-server-async.cpp src/daytime.hpp)
add_exercises(udp-daytime-client-sync)
add_exercises(udp-daytime-server-sync SOURCE src/udp-daytime-server-sync.cpp src/daytime.hpp)
add_exercises(udp-daytime-server-async SOURCE src/udp-daytime-server-async.cpp src/daytime.hpp)
add_exercises(combined-daytime-server-async SOURCE src/combined-daytime-server-async.cpp src/daytime.hpp)
