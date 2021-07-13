#include <fructose/fructose.h>
#include <ctime>
#include <iostream>
#include "FuncInterp.hpp"

struct testFuncInterp : public fructose::test_base< testFuncInterp >{

   void testSBO(const std::string& name){
      testStr("2+2", "((2)+(2))");
      testStr("2-2", "((2)-(2))");
      testStr("2*2", "((2)*(2))");
      testStr("2/2", "((2)/(2))");
      testStr("2+2-2+2", "((2)+((2)-((2)+(2))))");
      testStr("2*2/2*2", "((2)*((2)/((2)*(2))))");
      testStr("2^2^2^2", "((2)^((2)^((2)^(2))))");
      testStr("1+3*4", "((1)+((3)*(4)))");
      testStr("1+3*4+2", "((1)+(((3)*(4))+(2)))");
      testStr("1+3*4*5+2", "((1)+(((3)*((4)*(5)))+(2)))");
      testStr("1+3*4*5+2*6", "((1)+(((3)*((4)*(5)))+((2)*(6))))");
      testStr("1+3*4*5+2*6+7", "((1)+(((3)*((4)*(5)))+(((2)*(6))+(7))))");
      testStr("1-3/4*5+2*6-7", "((1)-(((3)/((4)*(5)))+(((2)*(6))-(7))))");
      testStr("1+2*3^4*5+6", "((1)+(((2)*(((3)^(4))*(5)))+(6)))");
      testStr("1+2*3*4^5^6*7+8", "((1)+(((2)*((3)*(((4)^((5)^(6)))*(7))))+(8)))");
      testStr("1+2*3^4*5*6^7+8", "((1)+(((2)*(((3)^(4))*((5)*((6)^(7)))))+(8)))");
   }

   void testParentheses(const std::string& name){
      testStr("1+(3*4)", "((1)+((3)*(4)))");
      testStr("(1+3)*4", "(((1)+(3))*(4))");
      testStr("1-3/(4*5+2)*6-7", "((1)-(((3)/(((4)*(5))+(2)))*(6))-(7))");
      testStr("2^(4*(5+6)/2)", "((2)^((4)*(((5)+(6))/(2))))");
      testStr("(2+(2+(3*4)^(2+1)))*(5-(5-(6*7)^(8*4)))", "((2)+((2)+((3)*(4))^((2)+(1))))*((5)-((5)-((6)*(7))^((8)*(4))))");
      testStr("1+(2+(3+(4+(5^6))))", "(1)+((2)+((3)+((4)+((5)^(6)))))");
   }

   void testSpaces(const std::string& name){
      testStr("1 +3* 4", "((1)+((3)*(4)))");
      testStr(" 1 + 3 * 4 * 5 + 2 * 6 ", "((1)+(((3)*((4)*(5)))+((2)*(6))))");
      testStr("   1     +    3  * 4 *5+ 2* 6  ", "((1)+(((3)*((4)*(5)))+((2)*(6))))");
      testStr(" 1   -  3  /  (  4  *  ( 5  + 2 ) )    *     6 - 7 ", "((1)-(((3)/(((4)*((5)+(2)))*(6)))-(7)))");
   }

   void testVariable(const std::string& name){
      testStr("x+3*x*5+x", "((x)+(((3)*((x)*(5)))+(x)))");
      testStr("x+x*x^x*x+x", "((x)+(((x)*(((x)^(x))*(x)))+(x)))");
      testStr("t+3*t*5+t", "((t)+(((3)*((t)*(5)))+(t)))");
      testStr("t+t*t^t*t+t", "((t)+(((t)*(((t)^(t))*(t)))+(t)))");
   }

   void testErrors(const std::string& name){
      testErrStr("", "Malformed function: unnecesary parentheses, or empty function");
      testErrStr("()", "Malformed function: unnecesary parentheses, or empty function");
      testErrStr("())", "Malformed function: unnecesary parentheses, or empty function");
      //testErrStr("3*", "Malformed function: unnecesary parentheses, or empty function");
      //testErrStr("(3*2)-", "Malformed function: unnecesary parentheses, or empty function");
      testErrStr("*(x^3)", "Malformed function: invalid variable or value");
   }

   void testErrStr(string totest, string result){
      string errmsg;
      FuncTree* tree = FuncTree::fromString(totest, &errmsg);
      fructose_assert(tree == NULL);
      fructose_assert_eq(errmsg, result);
   }

   void testStr(string totest, string result){
      string errmsg;
      FuncTree* tree = FuncTree::fromString(totest, &errmsg);
      fructose_assert(tree != NULL);
      fructose_assert_eq(tree->repr(), result);
      delete tree;
   }
};

int main(int argc, char **argv){
   testFuncInterp tester;
   tester.add_test("Binary Operations", &testFuncInterp::testSBO);
   tester.add_test("expression with unnessecary spaces", &testFuncInterp::testSpaces);
   tester.add_test("correct variable identification", &testFuncInterp::testVariable);
   tester.add_test("error and error messages", &testFuncInterp::testErrors);
   tester.add_test("parentheses", &testFuncInterp::testParentheses);
   return tester.run(argc, argv);
}
