#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/system/error_code.hpp>

namespace asio = boost::asio;
using boost::system::error_code;
using boost::posix_time::seconds;

void print(const error_code&) {
  std::cout << "Hello world!" << std::endl;
}

int main() {
  asio::io_service io;
  asio::deadline_timer t(io, seconds(5));

  t.async_wait(print);
  io.run();
  return 0;
}
