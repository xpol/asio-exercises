#include <chrono>
#include <iostream>
#include <system_error>

#include <asio.hpp>

using std::error_code;
using std::chrono::seconds;

void print(const error_code&) {
  std::cout << "Hello world!" << std::endl;
}

int main() {
  asio::io_service io;
  asio::steady_timer t(io, seconds(5));

  t.async_wait(print);
  io.run();
  return 0;
}
