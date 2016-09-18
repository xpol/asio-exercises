#include <iostream>
#include <array>
#include <boost/asio.hpp>

#include "daytime.hpp"

namespace asio = boost::asio;
using asio::ip::udp;
using boost::system::error_code;


class udp_server
{
public:
  explicit udp_server(asio::io_service& io): socket_(io, udp::endpoint(udp::v4(), 13))
  {
    start_receive();
  }

private:
  void start_receive()
  {
    socket_.async_receive_from(asio::buffer(recv_buffer_), remote_endpoint_, [this](const error_code& error, size_t bytes_transferred) {
      handle_receive(error, bytes_transferred);
    });
  }
  void handle_receive(const error_code& error, size_t bytes_transferred) {
    if (!error || error == asio::error::message_size) {
      std::shared_ptr<std::string> message(new std::string(make_daytime_string()));
      socket_.async_send_to(asio::buffer(*message), remote_endpoint_, [this, message](const error_code& error, size_t bytes_transferred) {
        handle_send(message, error, bytes_transferred);
      });
    }
    start_receive();
  }
  void handle_send(std::shared_ptr<std::string> message, const error_code& error, std::size_t bytes_transferred) {
    if (!error)
      std::cout << "Responsed " << bytes_transferred << " bytes." << std::endl;
  }
  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  std::array<char, 1> recv_buffer_;
};

int main()
{
  try
  {
    asio::io_service io;
    udp_server server(io);
    io.run();
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
