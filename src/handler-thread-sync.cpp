#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace asio = boost::asio;
using boost::system::error_code;
using boost::posix_time::seconds;

class printer {
public:
  printer(asio::io_service& io)
    : strand_(io),
    timer1_(io, seconds(1)),
    timer2_(io, seconds(1)),
    count_(0)
  {
    timer1_.async_wait(strand_.wrap([&](const error_code& ){
      print1();
    }));
    timer2_.async_wait(strand_.wrap([&](const error_code& ){
      print2();
    }));
  }

  ~printer() {
    std::cout << "Final count is " << count_ << std::endl;
  }

private:
  void print1() {
    if (count_ < 10) {
      std::cout << "Timer 1: " << count_ << std::endl;
      ++count_;
      timer1_.expires_at(timer1_.expires_at() + seconds(1));
      timer1_.async_wait(strand_.wrap([&](const error_code& ){
        print1();
      }));
    }
  }

  void print2() {
    if (count_ < 10) {
      std::cout << "Timer 2: " << count_ << std::endl;
      ++count_;
      timer2_.expires_at(timer2_.expires_at() + seconds(1));
      timer2_.async_wait(strand_.wrap([&](const error_code& ){
        print2();
      }));
    }
  }

  asio::io_service::strand strand_;
  asio::deadline_timer timer1_;
  asio::deadline_timer timer2_;
  int count_;
};

int main() {
  asio::io_service io;
  printer p(io);
  std::thread t([&](){
    io.run();
  });
  io.run();
  t.join();

  return 0;
}
