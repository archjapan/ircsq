#ifndef IRC_PREFIX_H_
#define IRC_PREFIX_H_
#include<string>

namespace irc {

class Prefix {
  public:
  // <nick>!<user>@<host>
  Prefix(const std::string& prefix) {
    using namespace std;
    size_t current = 0, found;
    if ((found = prefix.find_first_of('!', current)) != string::npos) {
      nick_ = string(prefix, current, found - current);
      current = found + 1;
    }
    if ((found = prefix.find_first_of('@', current)) != string::npos) {
      user_ = string(prefix, current, found - current);
      current = found + 1;
    }
    host_ = string(prefix, current, found - current);
  }
  std::string nick() const { return nick_; }
  std::string user() const { return user_; }
  std::string host() const { return host_; }

 private:
  std::string nick_;
  std::string user_;
  std::string host_;
};

} // namespace irc
#endif // IRC_PREFIX_H_
