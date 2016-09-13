#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace asio = boost::asio;
using boost::posix_time::seconds;
using boost::system::error_code;

void print(const error_code& , asio::deadline_timer* t, int* count) {
	if (*count < 5) {
		std::cout << *count << std::endl;
		++(*count);
		t->expires_at(t->expires_at() + seconds(1));
		t->async_wait([=](const error_code& e) {
			print(e, t, count);
		});
	}
}

int main() {
	asio::io_service io;
	auto count = 0;
	asio::deadline_timer t(io, seconds(1));
	t.async_wait([&](const error_code& e) {
		print(e, &t, &count);
	});
	io.run();
	return 0;
}
