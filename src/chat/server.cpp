#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>

#include <boost/asio.hpp>

#include "message.hpp"

namespace asio = boost::asio;
using asio::ip::tcp;
using boost::system::error_code;

typedef std::deque<message> message_queue;

class participant {
public:
  virtual ~participant() {}
  virtual void deliver(const message& msg) = 0;
};

typedef std::shared_ptr<participant> participant_ptr;

class room {
public:
  room() = default;
  room(const room&) = delete;
  room(room&&) = delete;
  room& operator=(const room&) = delete;
  room& operator=(room&&) = delete;

  void join(participant_ptr participant) {
    participants_.insert(participant);
    for (auto msg: recent_msgs_) {
      participant->deliver(msg);
    }
  }

  void leave(participant_ptr participant) {
    participants_.erase(participant);
  }

  void delever(const message& msg) {
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs) {
      recent_msgs_.pop_front();
    }
    for (auto participant: participants_) {
      participant->deliver(msg);
    }
  }
private:
  std::set<participant_ptr> participants_;
  message_queue recent_msgs_;
  static constexpr int max_recent_msgs = 100;
};


class session: public participant,
  public std::enable_shared_from_this<session> {
public:
  session(asio::io_service& io, room& room)
    : socket_(io), room_(room) {

  }
  tcp::socket& socket() {
    return socket_;
  }
  void start() {
    room_.join(shared_from_this());
    do_read_header();
  }

  void deliver(const message& msg) override {
    auto write_in_porgress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_porgress) {
      do_write();
    }
  }  
private:
  void do_read_header() {
    auto self(shared_from_this());
    auto buffer = header_buffer(read_msg_);
    asio::async_read(socket_, buffer, [this, self](error_code error, std::size_t /*length*/)
    {
      if (!error && read_msg_.decode_header()) {
        do_read_body();
      }
      else {
        room_.leave(shared_from_this());
      }
    });
  }
  void do_read_body() {
    auto self(shared_from_this());
    auto buffer = body_buffer(read_msg_);
    asio::async_read(socket_, buffer, [this, self](error_code error, std::size_t /*length*/)
    {
      if (!error) {
        room_.delever(read_msg_);
        do_read_header();
      }
      else {
        room_.leave(shared_from_this());
      }
    });
  }
  void do_write() {
    auto self(shared_from_this());
    auto buffer = message_buffer(write_msgs_.front());
    asio::async_write(socket_, buffer, [this, self](error_code error, std::size_t /*length*/)
    {
      if (!error) {
        write_msgs_.pop_front();
        if (!write_msgs_.empty()) {
          do_write();
        }
      }
      else {
        room_.leave(shared_from_this());
      }
    });
  }
  tcp::socket socket_;
  room& room_;
  message read_msg_;
  message_queue write_msgs_;
};

class server {
public:
  server(asio::io_service& io, const tcp::endpoint& endpoint) : acceptor_(io, endpoint)  {
    do_accept();
  }

private:
  void do_accept() {
    auto s = std::make_shared<session>(acceptor_.get_io_service(), room_);
    acceptor_.async_accept(s->socket(), [this, s](error_code error)
    {
      if (!error) {
        auto entpoint = s->socket().remote_endpoint();
        std::cout<< "New connection from: " << entpoint.address().to_string() << ":" << entpoint.port() << std::endl;
        s->start();
      }
      do_accept();
    });
  }

  tcp::acceptor acceptor_;
  room room_;
};

int main(int argc, char* argv[]) {
  try {
    if (argc < 2) {
      std::cerr << "Usage: chat-server <port> [<port> ...]" << std::endl;
      return 1;
    }
    asio::io_service io;
    std::list<server> servers;
    for (auto i = 1; i < argc; ++i) {
      tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
      servers.emplace_back(io, endpoint);
    }
    io.run();
  }
  catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
  return 0;
}
