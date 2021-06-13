#ifndef FUNCINTERP_HPP
#define FUNCINTERP_HPP

#include <string>
#include <stack>
#include <string.h>
#include <iterator>
#include <stdexcept>
#include <iostream>
#include <sstream> //should add preprocessor to remove from release build
#include <exception>

using std::string;

class FuncTree{
public:
   constexpr static char operators[] = "+-*/^";
   constexpr static int precedence[] = {0,0,1,1,2};
   constexpr static char variables[] = "xt";
   constexpr static char numericals[] = "123456780";

   FuncTree *lchild;
   FuncTree *rchild;
   void *val;
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

};

enum expected{
   LVAL,
   OP,
   RVAL
};

class function_structure : std::exception{
   string msg;
public:
   function_structure(const string &msg){
      this->msg = msg;
   }

   const char* what(){
      return msg.c_str();
   }
};

#endif
