#include <fructose/fructose.h>
#include <ctime>
#include <iostream>
#include "FuncInterp.hpp"

/*
To Test:
   -Unbalanced parentheses
   -
*/

struct testFuncInterp : public fructose::test_base< testFuncInterp >{

   void testSBO(const std::string& name){
      testStr("2+2", "((2)+(2))");
      testStr("2-2", "((2)-(2))");
      testStr("2*2", "((2)*(2))");
      testStr("2/2", "((2)/(2))");
      testStr("2+2-2+2", "((((2)+(2))-(2))+(2))");
      testStr("2*2/2*2", "((((2)*(2))/(2))*(2))");
      testStr("2^2^2^2", "((((2)^(2))^(2))^(2))");
      testStr("1+3*4", "((1)+((3)*(4)))");
      testStr("1+3*4+2", "(((1)+((3)*(4)))+(2))");
      testStr("1+3*4*5+2", "(((1)+(((3)*(4))*(5)))+(2))");
      testStr("1+3*4*5+2*6", "(((1)+(((3)*(4))*(5)))+((2)*(6)))");
      testStr("1+3*4*5+2*6+7", "((((1)+(((3)*(4))*(5)))+((2)*(6)))+(7))");
      testStr("1-3/4*5+2*6-7", "((((1)-(((3)/(4))*(5)))+((2)*(6)))-(7))");
      testStr("1+2*3^4*5+6", "(((1)+(((2)*((3)^(4)))*(5)))+(6))");
      testStr("1+2*3*4^5^6*7+8", "(((1)+((((2)*(3))*(((4)^(5))^(6)))*(7)))+(8))");
      testStr("1+2*3^4*5*6^7+8", "(((1)+((((2)*((3)^(4)))*(5))*((6)^(7))))+(8))");
      testStr("6/7/8", "(((6)/(7))/(8))");
      testStr("7*8-9", "(((7)*(8))-(9))");
   }

   void testParentheses(const std::string& name){
      testStr("1+(3*4)", "((1)+((3)*(4)))");
      testStr("(1+3)*4", "(((1)+(3))*(4))");
      testStr("1-3/(4*5+2)*6-7", "(((1)-(((3)/(((4)*(5))+(2)))*(6)))-(7))");
      testStr("2^(4*(5+6)/2)", "((2)^(((4)*((5)+(6)))/(2)))");
      testStr("(2+(2+(3*4)^(2+1)))*(5-(5-(6*7)^(8*4)))", "(((2)+((2)+(((3)*(4))^((2)+(1)))))*((5)-((5)-(((6)*(7))^((8)*(4))))))");
      testStr("1+(2+(3+(4+(5^6))))", "((1)+((2)+((3)+((4)+((5)^(6))))))");
   }

   void testSpaces(const std::string& name){
      testStr("1 +3* 4", "((1)+((3)*(4)))");
      testStr(" 1 + 3 * 4 * 5 + 2 * 6 ", "(((1)+(((3)*(4))*(5)))+((2)*(6)))");
      testStr("   1     +    3  * 4 *5+ 2* 6  ", "(((1)+(((3)*(4))*(5)))+((2)*(6)))");
      testStr(" 1   -  3  /  (  4  *  ( 5  + 2 ) )    *     6 - 7 ", "(((1)-(((3)/((4)*((5)+(2))))*(6)))-(7))");
   }

   void testVariable(const std::string& name){
      testStr("x+3*x*5+x", "(((x)+(((3)*(x))*(5)))+(x))");
      testStr("x+x*x^x*x+x", "(((x)+(((x)*((x)^(x)))*(x)))+(x))");
      testStr("t+3*t*5+t", "(((t)+(((3)*(t))*(5)))+(t))");
      testStr("t+t*t^t*t+t", "(((t)+(((t)*((t)^(t)))*(t)))+(t))");
   }

   void testErrors(const std::string& name){
      testErrStr("", "Malformed function");
      testErrStr("()", "Malformed function");
      testErrStr("())", "Malformed function");
      testErrStr("(()", "Malformed function");
      testErrStr("3*", "Malformed function");
      testErrStr("(3*2)-", "Malformed function");
      testErrStr("*(x^3)", "Malformed function: unrecognised variable or invalid value");
   }

   void testValues(const std::string& name){
      testStr("x", "(x)");
      testStr("-123", "(-123)");
      testStr("123.456", "(123.456)");
      testStr("-123.456", "(-123.456)");
      testStr(".123456", "(0.123456)");
      testStr("-.123456", "(-0.123456)");
      testStr("-0.01", "(-0.01)");
      testErrStr("-123.45.6", "Malformed function: unrecognised variable or invalid value");
      testErrStr("a", "Malformed function: unrecognised variable or invalid value");
   }

   void testEval(const std::string& name){
      testFunc("2+x", {-1,0,1,2,3}, {1,2,3,4,5});
      testFunc("5*x + 10", {-2,-1,0,1,2}, {0, 5, 10, 15, 20});
      testFunc("x^2", {-2,-1,0,1,2}, {4,1,0,1,4});
      testFunc("1/x", {10000, 1000, 100, 10, 1}, {.0001, .001, .01, .1, 1});
      testFunc("x/2 + 3", {2, 4, 6, 8, 10, 12}, {4, 5, 6, 7, 8, 9});
      testFunc("x*2 + 8 - (x/3)", {6, 12, 18, 24, 30}, {18, 28, 38, 48, 58});
      testFunc("4*3+6 - (200*1-50)", {1,2,3,4,5,6}, {-132, -132, -132, -132, -132, -132});
   }

   void testFunc(string totest, const std::vector<double> &input, const std::vector<double> &output){
      string errmsg;
      FuncTree* tree = FuncTree::fromString(totest, &errmsg);
      fructose_assert(tree != NULL);
      int pos = 0;
      while(pos < input.size()){
         fructose_assert_double_eq(tree->evaluate(input.at(pos)), output.at(pos));
         pos++;
      }
      delete tree;
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
      if(tree == NULL){
         std::cout << errmsg << std::endl;
      }
      fructose_assert(tree != NULL);
      fructose_assert_eq(tree->repr(), result);
      delete tree;
   }
};

int main(int argc, char **argv){
   testFuncInterp tester;
   tester.add_test("binary operations", &testFuncInterp::testSBO);
   tester.add_test("expression with unnessecary spaces", &testFuncInterp::testSpaces);
   tester.add_test("correct variable identification", &testFuncInterp::testVariable);
   tester.add_test("errors and error messages", &testFuncInterp::testErrors);
   tester.add_test("balanced parentheses", &testFuncInterp::testParentheses);
   tester.add_test("interesting values", &testFuncInterp::testValues);
   tester.add_test("evaluation of function", &testFuncInterp::testEval);
   return tester.run(argc, argv);
}
