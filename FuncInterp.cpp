#include "FuncInterp.hpp"

//TODO
// -negation
// -sin, cos, ln, e, pi
// -impliciti multiplication

constexpr char FuncTree::operators[6];
constexpr int FuncTree::precedence[5];
constexpr char FuncTree::variables[3];
constexpr char FuncTree::numericals[10];
//increments begin to character beyond last character in the found number
double FuncTree::doublefromString(
   std::string::iterator &begin,
   string &func){
   unsigned ibegin = std::distance(func.begin(), begin);
   unsigned end = ibegin;
   bool decimal_flag = false;
   while(end < func.length() && (strchr(numericals, func[end]) != NULL || func[end] == '.')){
      if(func[end] == '.'){
         if(decimal_flag){
            throw std::invalid_argument("doubleFromString");
         }
         decimal_flag = true;
      }
      end++;
      begin++;
   }
   return std::stod(func.substr(ibegin, end - ibegin), NULL);
}

inline int FuncTree::expectValue(
   FuncTree*& valNode,
   string::iterator &iter,
   string &func)
{
   if(strchr(variables, *iter) != NULL){
      valNode = new FuncTree(*iter, false);
   }
   else{
      try{
         valNode = new FuncTree(doublefromString(iter, func));
         iter--;
      }
      catch(std::invalid_argument& ive){
         throw function_structure("Malformed function: invalid variable or value");
      }
   }
   return 0;
}

//add flag to allow implicit multiplication
inline int FuncTree::expectOperator(char &op, string::iterator &iter){
   int cprec, pos;

   if((pos = findChar(operators, *iter)) > -1){
      op = *iter;
      cprec = precedence[pos];
   }
   else{
      throw function_structure("Malformed function: expected explicit operator");
   }
   return cprec;
}

inline void FuncTree::insertOperation(
   std::stack<FuncTree* > &funcs,
   char op,
   FuncTree *&root)
{
   int prec = precedence[findChar(operators, op)];

   while(funcs.size() > 0 && precedence[findChar(operators, funcs.top()->getVal_char())] >= prec){
      funcs.pop();
   }
   if(funcs.size() == 0){
      auto tmp = new FuncTree(op);
      tmp->lchild = root;
      funcs.push(tmp);
      root = tmp;
   }
   else{
      auto tmp = new FuncTree(op);
      tmp->lchild = funcs.top()->rchild;
      funcs.top()->rchild = tmp;
      funcs.push(tmp);
   }
}

FuncTree* FuncTree::sub_func(string::iterator &it, string &func){
   std::stack<FuncTree* > funcs;
   funcs.push(new FuncTree('\0', true));
   FuncTree* root = funcs.top();
   expected state = expected::LVAL;

   consumeSpaces(it);

   while(*it != ')'){

      switch(state){
         case expected::LVAL:
            if(*it == '('){\
               it++;
               funcs.top()->lchild = sub_func(it, func);
            }
            else expectValue(funcs.top()->lchild, it, func);
            state = expected::OP;

         break;
         case expected::OP:
            if(*it == '('){
               delete root;
               throw function_structure("Malformed function: implicit multiplication not supported");
            }

            char op;
            expectOperator(op, it);
            if(funcs.top()->getVal_char() == '\0'){
               funcs.top()->updateValue(op);
            }
            else{
               insertOperation(funcs, op, root);
            }
            state = expected::RVAL;

         break;
         case expected::RVAL:
            if(*it == '('){
               it++;
               funcs.top()->rchild = sub_func(it, func);
            }
            else expectValue(funcs.top()->rchild, it, func);
            state = expected::OP;
         break;
      }
      it++;
      consumeSpaces(it);
   }

   if(it == func.end()){
      delete root;
      throw function_structure("Malformed function: imbalanced parentheses");
   }

   if(root != NULL && root->getVal_char() == '\0'){
      delete root;
      throw function_structure("Malformed function: unnecesary parentheses, or empty function");
   }

   return root;
}

FuncTree* FuncTree::fromString(string func, string *errmsg) noexcept{
   func.append(")");
   auto iter = func.begin();
   try{
      return sub_func(iter, func);
   }
   catch(function_structure &fse){
      *errmsg = fse.what();
      return NULL;
   }
   catch(std::bad_alloc &bae){
      *errmsg = bae.what();
      return NULL;
   }
   catch(...){
      *errmsg = "function: unexpected exeception";
      return NULL;
   }
}

