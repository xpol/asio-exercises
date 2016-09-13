#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace asio = boost::asio;
using boost::posix_time::seconds;

int main() {
	asio::io_service io;
	asio::deadline_timer t(io, seconds(5));

	t.wait();
	std::cout << "Hello world!" << std::endl;
	return 0;
}
