#ifndef IRC_BOT_H_
#define IRC_BOT_H_
#include<iterator>
#include<iostream>
#include<vector>
#include<string>
#include<tuple>
#include<boost/algorithm/string.hpp>
#include<boost/asio.hpp>
#include "prefix.h"

namespace irc {

class Bot {
 public:
  typedef std::tuple<std::string, std::string, std::vector<std::string> > Message;
  typedef std::vector<std::string> Params;

 public:
  Bot(boost::asio::io_service& io_service,
      boost::asio::ip::tcp::resolver::iterator iterator,
      std::string user,
      std::string nick,
      std::string host)
    : user_(user),
      nick_(nick),
      host_(host),
      logined_(false),
      io_service_(io_service),
      socket_(io_service_),
      it_(iterator)
  {
//    DoConnect(iterator);
  }
  virtual ~Bot() {}
  void run();
  void Start();

 public:
  static Message token(const std::string &str);
  void Join(const std::string&);

 private:
  void Connect();
  void OnConnect(const boost::system::error_code&);
  void DoConnect(boost::asio::ip::tcp::resolver::iterator);
  void Login();

  void StartAccept();

  void Write(const std::string &str);
  void Write(const char *str);
  void OnWrite(const boost::system::error_code&, std::size_t);
  void Receive();
  virtual void OnReceive(const boost::system::error_code&, std::size_t);

 protected:
  void OnPing(Params&);
  void PrivMsg(std::string&, std::string&);
  virtual void OnPrivmsg(std::string&, Params&);
  void Notice(std::string&, std::string&);
  virtual void OnNotice(std::string&, Params&);

 private:
  std::string user_;
  std::string nick_;
  std::string host_;
  std::vector<std::string> q_channel_;
  bool logined_;
  boost::asio::io_service& io_service_;
  boost::asio::ip::tcp::socket socket_;
  boost::asio::ip::tcp::resolver::iterator it_;
  boost::asio::streambuf receive_buffer_;
};

}
#endif // IRC_BOT_H_
