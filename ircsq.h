#ifndef IRCLUA_H_
#define IRCLUA_H_
#include<iterator>
#include<iostream>
#include<vector>
#include<string>
#include<tuple>
#include<boost/algorithm/string.hpp>
#include<boost/asio.hpp>
#include "irc/bot.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <string>
#include <iostream>
#include <squirrel.h>
#include <sqstdblob.h>
#include <sqstdsystem.h>
#include <sqstdio.h>
#include <sqstdmath.h>	
#include <sqstdstring.h>
#include <sqstdaux.h>
#ifdef SQUNICODE
#define scfprintf fwprintf
#define scfopen	_wfopen
#define scvprintf vfwprintf
#else
#define scfprintf fprintf
#define scfopen	fopen
#define scvprintf vfprintf
#endif

SQInteger quit(HSQUIRRELVM v);
void printfunc(HSQUIRRELVM v,const SQChar *s,...);
void errorfunc(HSQUIRRELVM v,const SQChar *s,...);
std::string printvar(HSQUIRRELVM v);
std::string exec(HSQUIRRELVM v, std::string str);

class IRCSq : public irc::Bot {
  typedef std::tuple<std::string, std::string, std::vector<std::string> > Message;
 public:
  IRCSq(boost::asio::io_service& io_service,
      boost::asio::ip::tcp::resolver::iterator iterator,
      std::string user,
      std::string nick,
      std::string host)
      : Bot(io_service, iterator, user, nick, host)
  {
	v_=sq_open(1024);
	sq_setprintfunc(v_,printfunc,errorfunc);

	sq_pushroottable(v_);
	sqstd_seterrorhandlers(v_);
  }
  virtual ~IRCSq() {
	sq_close(v_);
  }

 protected:
  virtual void OnPrivmsg(std::string&, Params&);

 private:
  HSQUIRRELVM v_;
};

#endif // IRCLUA_H_
