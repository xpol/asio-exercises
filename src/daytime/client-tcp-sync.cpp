#include <array>
#include <iostream>
#include <system_error>
#include <asio.hpp>

using asio::ip::tcp;
using std::error_code;

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: "<< argv[0] <<" <host>" << std::endl;
      return 1;
    }
    asio::io_service io;

    tcp::resolver resolver(io);
    tcp::resolver::query query(argv[1], "daytime");
    auto endpoint_iterator = resolver.resolve(query);

    tcp::socket socket(io);
    asio::connect(socket, endpoint_iterator);

    for (;;)
    {
      std::array<char, 128> buf;
      error_code error;

      auto len = socket.read_some(asio::buffer(buf), error);

      if (error == asio::error::eof)
        break;
      if (error)
        throw std::system_error(error);
      std::cout.write(buf.data(), len);
    }
  }
  catch(std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}
