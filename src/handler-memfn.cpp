#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class printer {
public:
	printer(boost::asio::io_service& io)
		: timer_(io, boost::posix_time::seconds(1)), count_(0)
	{
		timer_.async_wait([&](const boost::system::error_code& ){
			print();
		});
	}

	~printer() {
		std::cout << "Final count is " << count_ << std::endl;
	}

private:
	void print() {
		if (count_ < 5) {
			std::cout << count_ << std::endl;
			++count_;
			timer_.expires_at(timer_.expires_at() + boost::posix_time::seconds(1));
			timer_.async_wait([&](const boost::system::error_code& ){
				print();
			});
		}
	}

	boost::asio::deadline_timer timer_;
	int count_;
};

int main() {
	boost::asio::io_service io;
	printer p(io);
	io.run();
	return 0;
}
