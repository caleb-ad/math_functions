#ifndef FUNCINTERP_HPP
#define FUNCINTERP_HPP

#include <string>
#include <map>
#include <cctype>
#include <stack>
#include <iterator>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <exception>
#include <cmath>

#include "func_except.hpp"

using std::string;
typedef double(*f_operation)(double, double);
typedef double(*f_value)(double);

enum expected{
   LVAL,
   OP,
   RVAL
};

enum type{
   VALUE,
   VALUE_FUNC,
   OPERATOR,
   VARIABLE
};

class FuncTree{
private:
   const static char operators[];
   const static int precedence[];
   const static char variables[];
   const static std::map<string, double> constants;// = {"e", "pi"};
   const static std::map<string, f_value> functions;// = {"sin", "cos", "tan", "ln", "sqrt"};

   FuncTree *lchild;
   FuncTree *rchild;
   void *val;
   f_operation op_func = NULL;
   f_value val_func = NULL;
   type mode;

public:

   FuncTree(double d){
      this->val = new double(d);
      this->mode = type::VALUE;
      this->lchild = NULL;
      this->rchild = NULL;
   }

   FuncTree(char c, bool operation = true){
      this->lchild = NULL;
      this->rchild = NULL;
      this->val = new char(c);
      if(operation){
         this->mode = type::OPERATOR;
         this->op_func = getOpFunc(c);
      }else{
         this->mode = type::VARIABLE;
      }

   }

   FuncTree( double(*val_func)(double), string name){
      this->mode = type::VALUE_FUNC;
      this->val = new string(name);
      this->val_func = val_func;
      this->lchild = NULL;
      this->rchild = NULL;
   }

   ~FuncTree(){
      if(lchild != NULL)
         delete lchild;
      if(rchild != NULL)
         delete rchild;
      //could be switch
      if(this->mode == type::VALUE){
         delete (double*)this->val;
      }
      else if(this->mode == type::VALUE_FUNC){
         delete (string*)this->val;
      }
      else{
         delete (char*)this->val;
      }
      return;
   }

   void print(){
      std::cout << "(";
      if(lchild != NULL){
         lchild->print();
      }

      switch(this->mode){
         case VALUE:
            std::cout << *(double*)this->val;
            break;
         case VALUE_FUNC:
            std::cout << *(string*)this->val;
            break;
         default:
            std::cout << *(char*)this->val;
         break;
      }

      if(rchild != NULL)
         rchild->print();
      std::cout << ")";
   }

   string repr(){
      std::stringstream out;
      repr_help(out);
      return out.str();
   }

   double evaluate(double val){
      if(this->mode == type::VALUE){
         return *((double*)(this->val));
      }
      else if(this->mode == type::VALUE_FUNC){
         return val_func(this->lchild->evaluate(val));
      }
      else if(this->mode == type::OPERATOR){
         return op_func(this->lchild->evaluate(val), this->rchild->evaluate(val));
      }
      else{//this->mode == type::VARIABLE
         return val;
      }
   }

   static std::map<string, double> initConstants(){
      std::map<string, double> c;
      c["e"] = 2.71828182846;
      c["pi"] = 3.14159265359;
      c["m"] = .001;
      c["u"] = .000001;
      c["p"] = .000000001;
      return c;
   }

   static std::map<string, f_value> initFunctions(){
      std::map<string, f_value> f;
      f["sin"] = &std::sin;
      f["cos"] = &std::cos;
      f["tan"] = &std::tan;
      f["sqrt"] = &std::sqrt;
      f["ln"] = &std::log;
      f["log"] = &std::log;
      return f;
   }

   static FuncTree* fromString(string func, string *errmsg) noexcept;

private:

   static FuncTree* sub_func(string::iterator &it, string &func);

   static double doublefromString(std::string::iterator &begin, string &func);

   static void expectValue(FuncTree*& current, string::iterator &iter, string &func);

   static void expectOperator(char &op, string::iterator &iter);

   static void insertOperation(std::stack<FuncTree* > &funcs, char op, FuncTree *&root);

   static int findChar(const char *cstr, char c){
      int pos = 0;
      while(*(cstr + pos) != '\0'){
         if(c == *(cstr + pos))
            return pos;
         pos += 1;
      }
      return -1;
   }

   static void consumeSpaces(string::iterator &it){
      /* depends on last character in string always being ')'
      if string ends in spaces, can advance beyond end of string and
      cause undefined behavior */
      while(*it == ' ' /*&& *it != ')'*/) it++;
   }

   static f_operation getOpFunc(char c){
      switch(c){
         case '+': return &FuncTree::add;
         case '-': return &FuncTree::subtract;
         case '*': return &FuncTree::multiply;
         case '/': return &FuncTree::divide;
         case '^': return &FuncTree::exponentiation;
         default: return NULL;
      }
   }

   static string getSymbol(string::iterator &it){
      //symbols can only have english alphabetic characters
      auto start = it;
      while(isalpha(*it)) it++;
      return string(start, it);
   }

   void updateOperation(char c){
      if(mode != type::OPERATOR){
         throw std::invalid_argument("Parse Error: attempt to update non operator");
      }
      else{
         *(char*)this->val = c;
         this->op_func = getOpFunc(c);
      }
   }

   char getOperation(){
      if(this->mode == type::OPERATOR){
         return *((char*)this->val);
      }
      throw function_structure("Parse Error: attempt to get operation of non-operator");
   }

   void repr_help(std::stringstream &out){
      out << "(";
      if(lchild != NULL){
         lchild->repr_help(out);
      }

      switch(this->mode){
         case VALUE:
            out << *(double*)this->val;
            break;
         case VALUE_FUNC:
            out << *(string*)this->val;
            break;
         default:
            out << *(char*)this->val;
         break;
      }

      if(rchild != NULL)
         rchild->repr_help(out);
      out << ")";
   }

   static double add(double a, double b){
      return a + b;
   }

   static double subtract(double a, double b){
      return a - b;
   }

   static double multiply(double a, double b){
      return a * b;
   }

   static double divide(double a, double b){
      return a / b;
   }

   static double exponentiation(double a, double b){
      return std::pow(a, b);
   }

};

#endif
