#include <chrono>
#include <iostream>
#include <system_error>

#include <asio.hpp>


using std::chrono::seconds;
using std::error_code;

class printer {
public:
  explicit printer(asio::io_service& io)
    : timer_(io, seconds(1)), count_(0)
  {
    timer_.async_wait([&](const error_code& ){
      print();
    });
  }

  ~printer() {
    std::cout << "Final count is " << count_ << std::endl;
  }

private:
  void print() {
    if (count_ < 5) {
      std::cout << count_ << std::endl;
      ++count_;
      timer_.expires_at(timer_.expires_at() + seconds(1));
      timer_.async_wait([&](const error_code& ){
        print();
      });
    }
  }

  asio::steady_timer timer_;
  int count_;
};

int main() {
  asio::io_service io;
  printer p(io);
  io.run();
  return 0;
}
