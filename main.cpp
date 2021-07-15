#include <iostream>
#include <string>
#include "FuncInterp.hpp"



void inspectMode(){
   char input[100];
   while(1){
      string errmsg;
      std::cin.getline(input, 100);
      if(input[0] == 'q')
         break;
      FuncTree* func = FuncTree::fromString(std::string(input), &errmsg);
      if(func == NULL){
         std::cout << errmsg << std::endl;
      }
      else{
         func->print();
         std::cout << std::endl;
      }
      delete func;
   }
}

void evaluateMode(){
   char input[100];
   string errmsg;
   std::cin.getline(input, 100);
   FuncTree *func = FuncTree::fromString(std::string(input), &errmsg);
   if(func == NULL){
      std::cout << errmsg << std::endl;
   }
   else{
      while(1){
         std::cin.getline(input, 100);
         if(input[0] == 'q')
            break;
         std::cout << func->evaluate(std::stod(input)) << std::endl;
      }
   }
   delete func;
}

int main(int argc, char **argv){
   if(argc > 1 && argv[1][0] == 'i'){
      inspectMode();
   }
   else{
      evaluateMode();
   }
   return 0;
}
