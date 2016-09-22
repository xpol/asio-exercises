#include <chrono>
#include <iostream>
#include <asio.hpp>

using std::chrono::seconds;

int main() {
  asio::io_service io;
  asio::steady_timer t(io, seconds(5));

  t.wait();
  std::cout << "Hello world!" << std::endl;
  return 0;
}
