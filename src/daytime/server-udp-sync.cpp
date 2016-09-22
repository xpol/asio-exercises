#include <iostream>
#include <array>
#include <system_error>

#include <asio.hpp>
#include "daytime.hpp"

using asio::ip::udp;
using std::error_code;


int main()
{
  try
  {
    asio::io_service io;
    udp::socket socket(io, udp::endpoint(udp::v4(), 13));
    for (;;)
    {
      std::array<char, 1> recv_buf;
      udp::endpoint remote_endpoint;
      error_code error;
      socket.receive_from(asio::buffer(recv_buf), remote_endpoint, 0, error);
      if (error && error != asio::error::message_size)
        throw std::system_error(error);

      auto message = make_daytime_string();
      error_code ignored_error;
      socket.send_to(asio::buffer(message), remote_endpoint, 0, ignored_error);
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
