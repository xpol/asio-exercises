#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <system_error>

#include <asio.hpp>
#include "daytime.hpp"

using asio::ip::tcp;
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
    message_ = make_daytime_string();
    auto ptr = shared_from_this();
    asio::async_write(socket_, asio::buffer(message_), [ptr](const error_code& error, size_t bytes_transfered)
    {
      ptr->handle_write(error, bytes_transfered);
    });
  }

private:
  explicit tcp_connection(asio::io_service& io) : socket_(io)
  {
  }

  void handle_write(const error_code& /*error*/, size_t bytes_transfered)
  {
    std::cout << "Responsed " << bytes_transfered << " bytes." << std::endl;
  }


  tcp::socket socket_;
  std::string message_;
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

int main()
{
  try
  {
    asio::io_service io;
    tcp_server server(io);
    io.run();
  }
  catch(std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
