cmake_minimum_required(VERSION 3.0)

project(asio-exercises CXX)

find_package(Boost REQUIRED COMPONENTS system date_time)
add_compile_options(-std=c++11)
include_directories(${Boost_INCLUDE_DIRS})

function(add_exercises name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} ${Boost_LIBRARIES})
endfunction()

add_exercises(timer-sync)
add_exercises(timer-async)
add_exercises(handler-args)
add_exercises(handler-memfn)
