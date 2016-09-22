#include <array>
#include <ctime>
#include <iostream>
#include <memory>
#include <system_error>

#include <asio.hpp>
#include "daytime.hpp"

using asio::ip::tcp;
using asio::ip::udp;
using std::error_code;


class tcp_connection : public std::enable_shared_from_this<tcp_connection>
{
public:
  typedef std::shared_ptr<tcp_connection> pointer;
  static pointer create(asio::io_service& io)
  {
    return pointer(new tcp_connection(io));
  }

  tcp::socket& socket()
  {
    return socket_;
  }

  void start()
  {
    auto message = std::make_shared<std::string>(make_daytime_string());
    auto ptr = shared_from_this();
    asio::async_write(socket_, asio::buffer(*message), [ptr, message](const error_code& error, size_t bytes_transfered)
    {
      ptr->handle_write(message, error);
    });
  }

private:
  explicit tcp_connection(asio::io_service& io) : socket_(io)
  {
  }

  void handle_write(std::shared_ptr<std::string> message, const error_code& error)
  {
    if (!error)
      std::cout << "Responsed: " << *message << std::endl;
  }


  tcp::socket socket_;
};

class tcp_server
{
public:
  explicit tcp_server(asio::io_service& io)
    : acceptor_(io, tcp::endpoint(tcp::v4(), 13))
  {
    start_accept();
  }
private:
  void start_accept()
  {
    auto connection = tcp_connection::create(acceptor_.get_io_service());
    acceptor_.async_accept(connection->socket(), [this, connection](const error_code& error)
    {
      handle_accept(connection, error);
    });

  }
  void handle_accept(tcp_connection::pointer connection, const error_code& error)
  {
    if (!error)
    {
      connection->start();
    }
    start_accept();
  }
  tcp::acceptor acceptor_;
};


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
      auto message = std::make_shared<std::string>(make_daytime_string());
      socket_.async_send_to(asio::buffer(*message), remote_endpoint_, [this, message](const error_code& error, size_t bytes_transferred) {
        handle_send(message, error);
      });
    }
    start_receive();
  }
  void handle_send(std::shared_ptr<std::string> message, const error_code& error) {
    if (!error)
      std::cout << "Responsed: " << *message << std::endl;
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
    tcp_server tcpserver(io);
    udp_server udpserver(io);
    io.run();
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
