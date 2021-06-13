#include <iostream>
#include <string>
#include "FuncInterp.hpp"

int main(){
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

   }
   return 0;
}
