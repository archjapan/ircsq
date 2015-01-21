#include<boost/algorithm/string.hpp>
#include<boost/asio.hpp>
#include<iterator>
#include<iostream>
#include<string>
#include "ircsq.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>

void read_data(boost::property_tree::ptree &pt, std::string& str1, const std::string& str2) {
  if (boost::optional<std::string> value = pt.get_optional<std::string>(str2)) {
    str1 = value.get();
  }
}

int main() {
  using namespace std;
  using namespace boost::property_tree;
  namespace asio = boost::asio;
  namespace ip = asio::ip;

  ptree pt;
  string server, port, user, nick, host;
  vector<string> channels;
  
  read_json("config.json", pt);

  read_data(pt, server, "server");
  read_data(pt, port, "port");
  read_data(pt, user, "user");
  read_data(pt, nick, "nick");
  read_data(pt, host, "host");

  BOOST_FOREACH (const ptree::value_type& child, pt.get_child("channels")) {
    const ptree& i = child.second;
    channels.push_back(i.data());
  }

  asio::io_service io_service;
  
  ip::tcp::resolver resolver(io_service);
  ip::tcp::resolver::query q(server, port);
  ip::tcp::resolver::iterator iterator = resolver.resolve(q);

  IRCSq ircsq(io_service, iterator, user, nick, host);

  ircsq.Start();
  for(const string &c : channels) {
    ircsq.Join(c);
  }
  io_service.run();

  return 0;
}

