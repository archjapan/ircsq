#include<iterator>
#include<iostream>
#include<string>
#include<boost/algorithm/string.hpp>
#include<boost/bind.hpp>
#include<boost/asio.hpp>
#include "bot.h"

namespace irc {

void Bot::Write(const std::string &str) {
  namespace asio = boost::asio;

  std::string send_str;
  send_str = str + "\r\n";
  asio::async_write(
      socket_,
      asio::buffer(send_str),
      boost::bind(&Bot::OnWrite, this,
        asio::placeholders::error,
        asio::placeholders::bytes_transferred));
}

void Bot::Write(const char *str) {
  std::string send_str(str);
  Write(send_str);
}

void Bot::OnWrite(const boost::system::error_code& error,
    std::size_t bytes_transferred) {
  if (error) {
    std::cerr << "send failed: " << error.message() << std::endl;
  }
}

void Bot::Receive() {
  namespace asio = boost::asio;
  namespace ip = asio::ip;

  asio::async_read_until(
      socket_,
      receive_buffer_,
      "\r\n",
      boost::bind(&Bot::OnReceive, this,
        asio::placeholders::error,
        asio::placeholders::bytes_transferred));
}

void Bot::OnReceive(const boost::system::error_code& error,
    std::size_t bytes_transferred) {
  namespace asio = boost::asio;
  if (error && error != asio::error::eof )
  {
    std::cerr << "receive failed: " << error.message() << std::endl;
  } else {
    using namespace std;
    using namespace boost::algorithm;
    string str((istreambuf_iterator<char>(&receive_buffer_)),
      istreambuf_iterator<char>());
//    split(msg, str, is_any_of(" "), token_compress_on );
    Message msg = token(str);
    auto prefix  = get<0>(msg);
    auto command = get<1>(msg);
    auto params = get<2>(msg);

    if (command == "PING") {
      OnPing(params);
    } else if (command == "PRIVMSG") {
      OnPrivmsg(prefix, params); 
    } else if (command == "NOTICE") {
      OnNotice(prefix, params); 
    } else {
      cout << "\033[31m" << command << "\033[39m " << str << endl;
    }
    Receive();
  }
}

void Bot::OnPing(Params &params) {
  Write("PONG :"+params[0]);
}

void Bot::PrivMsg(std::string &channel, std::string &message) {
  Write("PRIVMSG "+channel+" :"+message);
}
void Bot::OnPrivmsg(std::string &prefix, Params &params) {
  Prefix p(prefix);
  std::string channel = params[0];
  std::string message = params[1];
  std::cout << "PRIVMSG " << channel << 
    " (" << p.nick() << ") " << message << std::endl;
}
void Bot::Notice(std::string &channel, std::string &message) {
  Write("NOTICE "+channel+" :"+message);
}

void Bot::OnNotice(std::string &prefix, Params &params) {
  Prefix p(prefix);
  std::string channel = params[0];
  std::string message = params[1];
  std::cout << "NOTICE " << channel << 
    " (" << p.nick() << ") " << message << std::endl;
}

void Bot::Start() {
  Connect();
}

Bot::Message Bot::token(const std::string &message) {
  using namespace std;

  char space = ' ';
  string prefix, command;
  vector<string> params;
  size_t current = 0, found;

  if (message[0] == ':') {
    if ((found = message.find_first_of(space, current)) != string::npos) {
      prefix = string(message, current+1, found - current);
      current = found + 1;
    } else {
    }
  } else {
    prefix = "";
  }

  if ((found = message.find_first_of(space, current)) != string::npos) {
    command = string(message, current, found - current);
    current = found + 1;
  } else {
  }

  while ((found = message.find_first_of(space, current)) != string::npos
      && message[current] != ':') {
    params.push_back(string(message, current, found - current));
    current = found + 1;
  }
  if (message[current] == ':') {
    params.push_back(string(message, current+1, message.size() - current));
  } else {
    params.push_back(string(message, current, message.size() - current));
  }
  return make_tuple(prefix, command, params);
}

void Bot::DoConnect(boost::asio::ip::tcp::resolver::iterator it) {
  namespace asio = boost::asio;
  namespace ip = asio::ip;
  connect(socket_, it);
  Login();
}

void Bot::Connect() {
  namespace asio = boost::asio;
  namespace ip = asio::ip;

  asio::async_connect(
      socket_,
      it_,
      boost::bind(&Bot::OnConnect, this, asio::placeholders::error));
}

void Bot::OnConnect(const boost::system::error_code& error) {
  if (error) {
    std::cout << "connect failed : " << error.message() << std::endl;
  }
  Login();
  logined_ = true;
  for(auto chan : q_channel_) {
    Join(chan);
  }
  Receive();
}

void Bot::Login() {
  using namespace std;
  string user = "USER " + nick_ + " " + host_ + " server " + user_;
  string nick = "NICK " + nick_;
  Write(user);
  Write(nick);
}

void Bot::Join(const std::string &channel) {
  if (logined_) {
    Write("JOIN " + channel);
  } else {
    q_channel_.push_back(channel);
  }
}

} // namespace irc
