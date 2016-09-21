#ifndef __CHAT_MESSAGE_HPP__
#define __CHAT_MESSAGE_HPP__

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <boost/asio/buffer.hpp>

class message {
public:
  static constexpr int header_length = 4;
  static constexpr int max_body_length = 512;

  message(): body_length_(0) {
  }

  const char* data() const {
    return data_;
  }

  char* data() {
    return data_;
  }

  const char* body() const {
    return data_ + header_length;
  }

  char* body() {
    return data_ + header_length;
  }

  std::size_t body_length() const {
    return body_length_;
  }

  void set_body_length(std::size_t new_length) {
    body_length_ = new_length > max_body_length ? max_body_length : new_length;
  }

  bool decode_header() {
    char header[header_length + 1] = { 0 };
    std::strncpy(header, data_, header_length);
    body_length_ = std::atoi(header);
    if (body_length_ > max_body_length) {
      body_length_ = 0;
      return false;
    }
    return true;
  }

  void encode_header() {
    char header[header_length + 1] = { 0 };
    std::sprintf(header, "%4d", static_cast<int>(body_length_));
    std::memcpy(data_, header, header_length);
  }
private:
  char data_[header_length + max_body_length];
  std::size_t body_length_;
};

inline auto header_buffer(message& msg) {
  return boost::asio::buffer(msg.data(), message::header_length);
}

inline auto body_buffer(message& msg) {
  return boost::asio::buffer(msg.body(), msg.body_length());
}

inline auto message_buffer(message& msg) {
  return boost::asio::buffer(msg.data(), msg.body_length()+message::header_length);
}

#endif
