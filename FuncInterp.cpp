#include "FuncInterp.hpp"

// TODO
// -functions that return an NAN or infinity
// -evaluate for multivariable
// -recognize specific cases of std::pow and other math and replace with relevant std::math function
// -pseudo-jit compilation
// -test insertOperation behavior, can it throw Parse Error?
// -properly find and report all errors
// -update print and repr
// -simplify and optimize algorithms
// -implicit multiplication
//    -between certain symbols when an operator is expected
//    -implicit multiplication by -1, sometimes subtract other times multiply
//    -between combinations of constants and functions ex) esin(x)
// -algorithm to simplify expression
// TOTEST
//  -More imbalanced parentheses
//  -Weird values, with weird characters

const char FuncTree::operators[] = "+-*/^";;
const int FuncTree::precedence[] = {0,0,1,1,2};
const char FuncTree::variables[] = "xyzt";
const std::map<string, double> FuncTree::constants = FuncTree::initConstants();
const std::map<string, f_value> FuncTree::functions = FuncTree::initFunctions();

//increments begin to character beyond last character in the found number
double FuncTree::doublefromString(
   std::string::iterator &begin,
   string &func)
{
   unsigned ibegin = std::distance(func.begin(), begin);
   unsigned end = ibegin;
   bool decimal_flag = false;

   if(end < func.length() && func[end] == '-'){
      end++;
      begin++;
   }

   while(end < func.length() && (isdigit(func[end]) || func[end] == '.')){
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

inline void FuncTree::expectValue(
   FuncTree*& valNode,
   string::iterator &iter,
   string &func)
{
   //matches variables last, because variables match only a single character
   string sym = getSymbol(iter);
   if(sym.size() > 0){
      auto iter1 = constants.find(sym);
      if(iter1 != constants.end()){
         valNode = new FuncTree(iter1->second);
      }
      else{
         auto iter2 = functions.find(sym);
         if(iter2 != functions.end()){
            valNode = new FuncTree(iter2->second, iter2->first);
            if(*iter != '('){
               throw function_structure("Malformed function: expected ( after function");
            }
            iter++;
            valNode->lchild = sub_func(iter, func);
            iter++;
         }
         else if(sym.size() == 1 && strchr(variables, sym.at(0)) != NULL){
            valNode = new FuncTree(sym.at(0), false);
         }
         else{
            throw function_structure("Malformed function: unrecognised variable, constant, or function");
         }
      }
   }
   else{
      try{
         valNode = new FuncTree(doublefromString(iter, func));
      }
      catch(...){
         throw function_structure("Malformed function: invalid value");
      }
   }
   iter--; //sub_func should always advance iter to the first unseen token
}

inline void FuncTree::expectOperator(char &op, string::iterator &iter){

   if(findChar(operators, *iter) > -1){
      op = *iter;
   }
   else{
      throw function_structure("Malformed function: expected explicit operator");
   }
}

inline void FuncTree::insertOperation(
   std::stack<FuncTree* > &funcs,
   char op,
   FuncTree *&root)
{
   int prec = precedence[findChar(operators, op)];

   while(funcs.size() > 0 && precedence[findChar(operators, funcs.top()->getOperation())] >= prec){
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
   funcs.push(new FuncTree('\0'));
   FuncTree* root = funcs.top();
   expected state = expected::LVAL;

   consumeSpaces(it);
   try{
   while(*it != ')'){
      if(it == func.end()){
         throw function_structure("Malformed function: imbalanced parentheses");
      }
      switch(state){
         case expected::LVAL:
            if(*it == '('){\
               it++;
               funcs.top()->lchild = sub_func(it, func);
            }else{
               expectValue(funcs.top()->lchild, it, func);
            }
            state = expected::OP;

         break;
         case expected::OP:
            char op;
            expectOperator(op, it);
            if(funcs.top()->getOperation() == '\0'){
               funcs.top()->updateOperation(op);
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
            }else{
               expectValue(funcs.top()->rchild, it, func);
            }
            state = expected::OP;
         break;
      }
      it++;
      consumeSpaces(it);
   }
   }catch(std::exception& exp){
      delete root;
      throw function_structure(exp.what());
   }
   //check for error conditions

   if(root == NULL || state == expected::LVAL || (root->rchild == NULL && state == expected::RVAL)){
      delete root;
      throw function_structure("Malformed function");
   }

   //for "functions" that are only a single value
   if(root->getOperation() == '\0'){
      return root->lchild;
   }

   return root;
}

FuncTree* FuncTree::fromString(string func, string *errmsg) noexcept{
   func.append(")");
   auto iter = func.begin();
   try{
      auto tmp = sub_func(iter, func);
      if(++iter != func.end()){
         delete tmp;
         *errmsg = "Malformed function: imbalanced parentheses";
         return NULL;
      }
      return tmp;
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

