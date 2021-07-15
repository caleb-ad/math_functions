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
      //testErrStr("", "Malformed function: unnecesary parentheses, or empty function");
      //testErrStr("()", "Malformed function: unnecesary parentheses, or empty function");
      //testErrStr("())", "Malformed function: unnecesary parentheses, or empty function");
      testErrStr("3*", "Malformed function: operator without rvalue");
      testErrStr("(3*2)-", "Malformed function: operator without rvalue");
      testErrStr("*(x^3)", "Malformed function: unrecognised variable or invalid value");
   }

   void testValues(const std::string& name){
      testStr("x", "(x)");
      testStr("-123", "(-123)");
      testStr("123.456", "(123.456)");
      testStr("-123.456", "(123.456)");
      testStr(".123456", "(.123456)");
      //testStr("-.123456", "(-.123456)");
      testErrStr("-123.45.6", "Malformed function: unrecognised variable or invalid value");
      testErrStr("y", "Malformed function: unrecognised variable or invalid value");
   }

   void testEval(const std::string& name){
      testFunc("2+x", {-1,0,1,2,3}, {1,2,3,4,5});
   }

   void testFunc(string totest, const std::vector<double> &input, const std::vector<double> &output){
      string errmsg;
      FuncTree* tree = FuncTree::fromString(totest, &errmsg);
      fructose_assert(tree != NULL);
      int pos = 0;
      while(pos < input.size()){
         fructose_loop1_assert(0, pos, tree->evaluate(input.at(pos)) == output.at(pos));
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
   return tester.run(argc, argv);
}
