#include <cstdlib>
#include <deque>
#include <iostream>
#include <system_error>
#include <thread>

#include <asio.hpp>
#include "message.hpp"

using asio::ip::tcp;
using std::error_code;

typedef std::deque<message> message_queue;

class client {
public:

  client(asio::io_service& io, tcp::resolver::iterator endpoint_iterator)
    : io_(io), socket_(io) {
    do_connect(endpoint_iterator);
  }

  void write(const message& msg) {
    io_.post([this, msg]()
    {
      auto write_in_progress = !write_msgs_.empty();
      write_msgs_.push_back(msg);
      if (!write_in_progress) {
        do_write();
      }
    });
  }

  void close() {
    io_.post([this]() {socket_.close(); });
  }
private:
  void do_connect(const tcp::resolver::iterator endpoint_iterator) {
    asio::async_connect(socket_, endpoint_iterator, [this](error_code error, const tcp::resolver::iterator /*endpoint_iterator*/)
    {
      if (!error) {
        do_read_header();
      }
    });
  }

  void do_read_header() {
    auto buffer = header_buffer(read_msg_);
    asio::async_read(socket_, buffer, [this](error_code error, std::size_t /*length*/)
    {
      if (!error && read_msg_.decode_header()) {
        do_read_body();
      }
      else {
        socket_.close();
      }
    });
  }

  void do_read_body() {
    auto buffer = body_buffer(read_msg_);
    asio::async_read(socket_, buffer, [this](error_code error, std::size_t /*length*/)
    {
      if (!error) {
        std::cout.write(read_msg_.body(), read_msg_.body_length());
        std::cout << std::endl;
        do_read_header();
      }
    });
  }

  void do_write() {
    auto buffer = message_buffer(write_msgs_.front());
    asio::async_write(socket_, buffer, [this](error_code error, std::size_t /*length*/)
    {
      if (!error) {
        write_msgs_.pop_front();
        if (!write_msgs_.empty()) {
          do_write();
        }
      }
      else {
        socket_.close();
      }
    });
  }

  asio::io_service& io_;
  tcp::socket socket_;
  message read_msg_;
  message_queue write_msgs_;
};

int main(int argc, char* argv[]) {
  try {
    if (argc != 3) {
      std::cerr << "Usage: chat-client <host> <port>" << std::endl;
      return 1;
    }
    asio::io_service io;
    tcp::resolver resolver(io);
    auto endpoint_iterator = resolver.resolve({ argv[1], argv[2] });
    client c(io, endpoint_iterator);
    std::thread t([&io]() {io.run(); });

    char line[message::max_body_length + 1];
    while (std::cin.getline(line, message::max_body_length + 1)) {
      message msg;
      msg.set_body_length(strlen(line));
      std::memcpy(msg.body(), line, msg.body_length());
      msg.encode_header();
      c.write(msg);
    }
  }
  catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
  return 0;
}
