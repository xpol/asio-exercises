#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

namespace asio = boost::asio;
using asio::ip::tcp;
using boost::system::error_code;

std::string make_daytime_string()
{
	using namespace std;
	auto now = time(0);
	return ctime(&now);
}

int main()
{
	try
	{
		asio::io_service io;
		tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 13));
		for (;;)
		{
			tcp::socket socket(io);
			acceptor.accept(socket);
			auto message = make_daytime_string();
			
			error_code ignore_error;
			asio::write(socket, asio::buffer(message), ignore_error);
		}
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}