#include <iostream>
#include <array>
#include <boost/asio.hpp>

namespace asio = boost::asio;
using asio::ip::udp;
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
		udp::socket socket(io, udp::endpoint(udp::v4(), 13));
		for (;;)
		{
			std::array<char, 1> recv_buf;
			udp::endpoint remote_endpoint;
			error_code error;
			socket.receive_from(asio::buffer(recv_buf), remote_endpoint, 0, error);
			if (error && error != asio::error::message_size)
				throw boost::system::system_error(error);

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