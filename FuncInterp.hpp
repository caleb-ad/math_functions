#ifndef FUNCINTERP_HPP
#define FUNCINTERP_HPP

#include <string>
#include <stack>
#include <string.h>
#include <iterator>
#include <stdexcept>
#include <iostream>
#include <sstream> //should add preprocessor to remove from a release build
#include <exception>
#include <cmath>

#include "func_except.hpp"

using std::string;

enum expected{
   LVAL,
   OP,
   RVAL
};

class FuncTree{
public:
   const static char operators[];
   const static int precedence[];
   const static char variables[];
   const static char numericals[];
   const static string constants[];// = {"e", "pi"};
   const static string functions[];// = {"sin", "cos", "tan", "ln", "sqrt"};

   FuncTree *lchild;
   FuncTree *rchild;
   void *val;
   double (*value) (double a, double b);
   unsigned char mode;

   FuncTree(double d){
      this->val = new double(d);
      this->mode = 0;
      this->lchild = NULL;
      this->rchild = NULL;
   }

   FuncTree(char c, bool operation = true){
      this->val = new char(c);
      this->lchild = NULL;
      this->rchild = NULL;
      if(operation){
         mode = 1;
      }else{
         mode = 2;
      }
   }

   ~FuncTree(){
      if(lchild != NULL)
         delete lchild;
      if(rchild != NULL)
         delete rchild;
      if(this->mode == 0){
         delete (double*)this->val;
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
         case 0:
            std::cout << *(double*)this->val;
            break;
         case 1:
            std::cout << *(char*)this->val;
            break;
         case 2:
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

   char getVal_char(){
      if(this->mode != 1 && this->mode != 2){
         throw std::out_of_range("node does not store char");
      }
      else{
         return *(char*)this->val;
      }
   }

   double getVal_double(){
      if(this->mode != 0){
         throw std::out_of_range("node does not store double");
      }
      else{
         return *(double*)this->val;
      }
   }

   double evaluate(double val){
      if(this->mode == 0){
         return *((double*)(this->val));
      }
      else if(this->mode == 2){
         return val;
      }
      else{
      switch(*((unsigned char*)(this->val))){
         case '+':
            return this->lchild->evaluate(val) + this->rchild->evaluate(val);
         break;
         case '-':
            return this->lchild->evaluate(val) - this->rchild->evaluate(val);
         break;
         case '*':
            return this->lchild->evaluate(val) * this->rchild->evaluate(val);
         break;
         case '/':
            return this->lchild->evaluate(val) / this->rchild->evaluate(val);
         break;
         case '^':
            return std::pow(this->lchild->evaluate(val), this->rchild->evaluate(val));
         break;
         default:
            throw function_structure("Function evaluation: invalid operation");
         break;
      }
      }

   }

   static FuncTree* fromString(string func, string *errmsg) noexcept;

private:

   static FuncTree* sub_func(string::iterator &it, string &func);

   static double doublefromString(std::string::iterator &begin, string &func);

   static int expectValue(FuncTree*& current, string::iterator &iter, string &func);

   static int expectOperator(char &op, string::iterator &iter);

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
      while(*it == ' ' && *it != ')') it++;
   }

   static bool isValue(FuncTree *tree){
      return tree != NULL && (tree->mode == 0 || tree->mode == 2);
   }

   void updateValue(double d){
      if(mode != 0){
         throw std::invalid_argument("attempt to update char node with double");
      }
      else{
         delete (double*)this->val;
         this->val = new double(d);
      }
   }

   void updateValue(char c){
      if(mode != 1 && mode != 2){
         throw std::invalid_argument("attempt to update non-double node with double");
      }
      else{
         delete (char*)this->val;
         this->val = new char(c);
      }
   }

   void repr_help(std::stringstream &out){
      out << "(";
      if(lchild != NULL){
         lchild->repr_help(out);
      }

      switch(this->mode){
         case 0:
            out << *(double*)this->val;
            break;
         case 1:
            out << *(char*)this->val;
            break;
         case 2:
            out << *(char*)this->val;
            break;
      }

      if(rchild != NULL)
         rchild->repr_help(out);
      out << ")";
   }

   double add(double a, double b){
      return a + b;
   }

   double subtract(double a, double b){
      return a - b;
   }

   double multiply(double a, double b){
      return a * b;
   }

   double divide(double a, double b){
      return a / b;
   }

   double exponentiation(double a, double b){
      return std::pow(a, b);
   }

};

#endif
