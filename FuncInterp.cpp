#include "FuncInterp.hpp"

//TODO
// -fix expect operator state error handling (is it possible to find a null operation when doing an insert operation)
// -test insertOperation behavior
// -evaluate for multivariable
// -sin, cos, ln, e, pi
// -properly find and report all errors
// -fix print null characters when function is single value
// -simplify and optimize algorithms
// -implicit multiplication
//    -between certain symbols when an operator is expected
//    -implicit multiplication by -1
// -test large values

const char FuncTree::operators[6] = "+-*/^";;
const int FuncTree::precedence[5] = {0,0,1,1,2};
const char FuncTree::variables[5] = "xyzt";
const char FuncTree::numericals[10] = "123456780";
const static string constants[] = {"e", "pi"};
const static string functions[] = {"sin", "cos", "tan", "ln", "sqrt"};

//increments begin to character beyond last character in the found number
double FuncTree::doublefromString(
   std::string::iterator &begin,
   string &func)
{
   unsigned ibegin = std::distance(func.begin(), begin);
   unsigned end = ibegin;
   bool decimal_flag = false;

   if(func[end] == '-'){
      end++;
      begin++;
   }

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

inline void FuncTree::expectValue(
   FuncTree*& valNode,
   string::iterator &iter,
   string &func)
{
   if(strchr(variables, *iter) != NULL){
      valNode = new FuncTree(*iter);
   }
   else{
      try{
         valNode = new FuncTree(doublefromString(iter, func));
         iter--;
      }
      catch(std::invalid_argument& ive){
         throw function_structure("Malformed function: unrecognised variable or invalid value");
      }
   }
}

//add implicit multiplication
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

   while(funcs.size() > 0){
      if(funcs.top()->getOperation(op) != 0){
         throw function_structure("Parse Error: unexpected null operation");
      }
      if(precedence[findChar(operators, op)] < prec){
         break;
      }
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

      switch(state){
         case expected::LVAL:
            if(*it == '('){\
               it++;
               funcs.top()->lchild = sub_func(it, func);
            }
            expectValue(funcs.top()->lchild, it, func);
            state = expected::OP;

         break;
         case expected::OP:
            char op;
            expectOperator(op, it);
            if(funcs.top()->getOperation() == '\0'){
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
            expectValue(funcs.top()->rchild, it, func);
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
   if(it == func.end()){
      delete root;
      throw function_structure("Malformed function: imbalanced parentheses");
   }

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

