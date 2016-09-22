#include <chrono>
#include <iostream>
#include <system_error>

#include <asio.hpp>

using std::chrono::seconds;
using std::error_code;

void print(const error_code& , asio::steady_timer* t, int* count) {
  if (*count < 5) {
    std::cout << *count << std::endl;
    ++(*count);
    t->expires_at(t->expires_at() + seconds(1));
    t->async_wait([=](const error_code& e) {
      print(e, t, count);
    });
  }
}

int main() {
  asio::io_service io;
  auto count = 0;
  asio::steady_timer t(io, seconds(1));
  t.async_wait([&](const error_code& e) {
    print(e, &t, &count);
  });
  io.run();
  return 0;
}
