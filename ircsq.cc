#include<iterator>
#include<iostream>
#include<string>
#include<boost/algorithm/string.hpp>
#include<boost/lexical_cast.hpp>
#include<boost/bind.hpp>
#include<boost/asio.hpp>
#include "ircsq.h"
#include "irc/prefix.h"

SQInteger quit(HSQUIRRELVM v)
{
	int *done;
	sq_getuserpointer(v,-1,(SQUserPointer*)&done);
	*done=1;
	return 0;
}

void printfunc(HSQUIRRELVM v,const SQChar *s,...)
{
	va_list vl;
	va_start(vl, s);
	scvprintf(stdout, s, vl);
	va_end(vl);
}

void errorfunc(HSQUIRRELVM v,const SQChar *s,...)
{
	va_list vl;
	va_start(vl, s);
	scvprintf(stderr, s, vl);
	va_end(vl);
}

std::string printvar(HSQUIRRELVM v)
{
  using namespace std;
  using namespace boost;
  string ret;
  SQInteger idx = -1;
  SQInteger int_;
  SQFloat   float_;
  SQBool    bool_;

  switch(sq_gettype(v, idx)) {
    case OT_INTEGER:
      sq_getinteger(v, idx, &int_);
      return lexical_cast<string>(int_);
    case OT_FLOAT:
      sq_getfloat(v, idx, &float_);
      return lexical_cast<string>(float_);
    case OT_BOOL:
      sq_getbool(v, idx, &bool_);
      return lexical_cast<string>(bool_);
    case OT_STRING:
    case OT_NULL:
      return "null";
    case OT_TABLE:
    case OT_ARRAY:
    case OT_USERDATA:
    case OT_CLOSURE:
    case OT_NATIVECLOSURE:
    case OT_GENERATOR:
    case OT_USERPOINTER:
    case OT_THREAD:
    case OT_FUNCPROTO:
    case OT_CLASS:
    case OT_INSTANCE:
    case OT_WEAKREF:
    case OT_OUTER:
    default:
      break;
  }
  return "";
}

std::string exec(HSQUIRRELVM v, std::string str)
{
  boost::algorithm::trim(str);
  std::string buf = "return (" + str +")";

  if(buf.size()>0){
    SQInteger oldtop=sq_gettop(v);
    if(SQ_SUCCEEDED(sq_compilebuffer(v,buf.c_str(),buf.size(),_SC("<stdin>"),SQFalse))){
      sq_pushroottable(v);
      if(SQ_SUCCEEDED(sq_call(v,1,SQTrue,SQTrue))) {
        return printvar(v);
      }
    } else if (SQ_SUCCEEDED(sq_compilebuffer(v,
            str.c_str(),str.size(),_SC("<stdin>"),SQTrue))) {
      sq_pushroottable(v);
      SQ_SUCCEEDED(sq_call(v,1,SQFalse,SQTrue));
    }
    sq_settop(v,oldtop);
  }
  return "";
}

void IRCSq::OnPrivmsg(std::string& prefix, Params& params) {
  irc::Prefix p(prefix);
  std::string channel = params[0];
  std::string message = params[1];
  if (channel[0] == '#') {
    std::string str = exec(v_, message);
    if (!str.empty()) Notice(channel, str);
  } else {
    std::cout << "PRIVMSG " << channel << 
      " (" << p.nick() << ") " << message << std::endl;
  }
}

