#ifndef FUNC_EXCEPT_HPP
#define FUNC_EXCEPT_HPP

#include <string.h>

using std::string;

class function_structure : std::exception{
   string msg;
public:
   function_structure(const string &msg){
      this->msg = msg;
   }

   const char* what() const noexcept override {
      return msg.c_str();
   }
};

#endif
