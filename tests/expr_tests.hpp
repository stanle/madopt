/*
 * Copyright 2014 National ICT Australia Limited (NICTA)
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <cxxtest/TestSuite.h>
#include "testmodel.hpp"
using namespace MadOpt;

class ExprTest: public CxxTest::TestSuite {
    public:
        void Tes(Expr c, string test, OPType type, bool isconstant=false){
            TS_ASSERT_EQUALS(c.toString(), test);
            TS_ASSERT_EQUALS(c.getType(), type);
            TS_ASSERT_EQUALS(c.isConstant(), isconstant);
        }

        void testSimple(){
           TestModel m;
            Var a = m.addVar("a");
            Var b = m.addVar("b");
            Tes(a, "a", OP_VAR_POINTER);
            Tes(b, "b", OP_VAR_POINTER);
            a += b;
            Tes(a, "a+b", OP_ADD);
            Tes(Expr(3), "3", OP_CONST, true);
        }

        void testConstant(){
           TestModel m;
            Var a = m.addVar("a");
            Var b = m.addVar("b");

            Expr c = 4;
            TS_ASSERT(c.isConstant());
            c += a;
            TS_ASSERT(!c.isConstant());

            Expr d = a + 4;
            TS_ASSERT(!d.isConstant());

            Expr e = 4 + a;
            TS_ASSERT(!e.isConstant());
        }

        void testAdd(){
           TestModel m;
            Var a = m.addVar("a");
            Var b = m.addVar("b");
            Tes(a+b, "a+b", OP_ADD);
            Expr c = a + b;
            Tes(c, "a+b", OP_ADD);
            Tes(c+b, "a+b+b", OP_ADD);
            Tes(c, "a+b", OP_ADD);
            Tes(a+1+a+5, "a+1+a+5", OP_ADD);
            Tes(a, "a", OP_VAR_POINTER);
            Tes(b, "b", OP_VAR_POINTER);
        }

        void testPlusEqual(){
           TestModel m;
            Var a = m.addVar("a");
            Var b = m.addVar("b");

            Expr c = 1 + a;
            c += b;
            Tes(c, "1+a+b", OP_ADD);

            c += 5;
            Tes(c, "1+a+b+5", OP_ADD);

            Expr z(5);
            z += a;
            Tes(z, "5+a", OP_ADD);

            Expr t(5);
            t += 4;
            Tes(t, "5+4", OP_ADD);

            Expr e(0);
            e += a;
            Tes(e, "a", OP_VAR_POINTER);
        }

        void testIsZero(){
            TS_ASSERT(Expr(0).isZero());
        }

        void testPlusZero(){
           TestModel m;
            Var a = m.addVar("a");

            Tes(a+0, "a", OP_VAR_POINTER);
            Tes(0+a, "a", OP_VAR_POINTER);
            Tes(Expr(0)+a, "a", OP_VAR_POINTER);
            Tes(0+2*a, "2*a", OP_MUL);
            Tes((2*a + 0)*3, "2*a*3", OP_MUL);
        }

        void testPlusConst(){
           TestModel m;
            Var a = m.addVar("a");
            Tes(Expr(4)+a+Expr(2), "4+a+2", OP_ADD);
        }

        void testMul(){
           TestModel m;
            Var a = m.addVar("a");
            Var b = m.addVar("b");

            Expr c = a*b;
            Tes(c, "a*b", OP_MUL);
            Tes(a*c, "a*a*b", OP_MUL);
            Tes(c, "a*b", OP_MUL);

            Tes(2*a*6*b, "2*a*6*b", OP_MUL);
            Tes(a*2, "a*2", OP_MUL);
            Tes(2*a, "2*a", OP_MUL);
            Tes(2*(6*a), "2*6*a", OP_MUL);

            Tes(a, "a", OP_VAR_POINTER);
            Tes(b, "b", OP_VAR_POINTER);
        }

        void testMulPlus(){
           TestModel m;
            Var a = m.addVar("a");
            Var b = m.addVar("b");

            Expr c = (a+b)*(a+b);

            Tes(a+b*a+b, "a+b*a+b", OP_ADD);
            Tes((a+b)*(a+b), "(a+b)*(a+b)", OP_MUL);
        }

        void testMulOne(){
           TestModel m;
            Var a = m.addVar("a");
            Tes((a+2)*1, "a+2", OP_ADD);
            Expr b(1);
            b *= (a+2);
            Tes(b, "a+2", OP_ADD);
        }

        void testMulZero(){
           TestModel m;
            Var a = m.addVar("a");
            Var b = m.addVar("b");

            Tes(0*b, "0", OP_CONST, true);
            Tes(b*0, "0", OP_CONST, true);
            Tes(a+0*b, "a", OP_VAR_POINTER);
        }

        void testMulEq(){
           TestModel m;
            Var a = m.addVar("a");
            Var b = m.addVar("b");

            Expr c = 2*a;
            c *= b;
            Tes(c, "2*a*b", OP_MUL);

            c *= 5;
            Tes(c, "2*a*b*5", OP_MUL);

            c *= 0;
            Tes(c, "0", OP_CONST, true);

            Expr z(5);
            z *= a;
            Tes(z, "5*a", OP_MUL);

            Expr t(5);
            t *= 4;
            Tes(t, "5*4", OP_MUL);

            Expr e(0);
            e *= a;
            Tes(e, "0", OP_CONST, true);
        }

        void testUnaryPlus(){
           TestModel m;
            Var a = m.addVar("a");
            Tes(+a+2, "a+2", OP_ADD);
        }

        void testUnaryNeg(){
           TestModel m;
            Var a = m.addVar("a");
            Var b = m.addVar("b");

            Tes(-a, "-1*a", OP_MUL);
            Tes(-a+b, "-1*a+b", OP_ADD);
            Tes(-Expr(5), "-1*5", OP_MUL);
        }

        void testNeg(){
           TestModel m;
            Var a = m.addVar("a");
            Var b = m.addVar("b");
            Tes(a-b, "a+-1*b", OP_ADD);
            Tes(-a-b, "-1*a+-1*b", OP_ADD);
        }

        void testNegCons(){
           TestModel m;
            Var a = m.addVar("a");
            Tes(a-5, "a+-5", OP_ADD);
            Tes(5-a, "5+-1*a", OP_ADD);
            Tes(Expr(5) - Expr(2), "5+-1*2", OP_ADD);
        }

        void testPow(){
           TestModel m;
            Var a = m.addVar("a");
            Var b = m.addVar("b");
            Tes(pow(a,5), "a^5", OP_POW);
            Tes(pow(a+b,5), "(a+b)^5", OP_POW);
            Tes(pow(a,1), "a", OP_VAR_POINTER);
            Tes(pow(a,0), "1", OP_CONST, true);
            Tes(pow(a,2), "a^2", OP_POW);
            Tes(pow(pow(a, 2), 3), "a^6", OP_POW);
        }

        void testDiv(){
           TestModel m;
            Var a = m.addVar("a");
            Var b = m.addVar("b");
            Tes(a/b, "a*(b^-1)", OP_MUL);
        }

        void testSIN(){
           TestModel m;
            Var a = m.addVar("a");
            Tes(sin(a), "sin(a)", OP_SIN); 
            Tes(2*sin(a), "2*sin(a)", OP_MUL); 
        }

  void testLN(){
    TestModel m;
    Var a = m.addVar("a");
    Tes(ln(a), "ln(a)", OP_LN); 
    Tes(2*ln(a), "2*ln(a)", OP_MUL); 
  }


  void testLOG2(){
    TestModel m;
    Var a = m.addVar("a");
    Tes(log2(a), "log2(a)", OP_LOG2); 
    Tes(2*log2(a), "2*log2(a)", OP_MUL); 
  }

        void testTutorial(){
           TestModel m;
            Var a = m.addVar("a");
            Var b = m.addVar("b");
            Var c = m.addVar("c");
            Tes((pow(a,2) + 1.5*a - 2)*cos(b) - c,
                    "((a^2)+1.5*a+-2)*cos(b)+-1*c", OP_ADD);
        }

        void testXEvaluation(){
           TestModel m;
            Var a = m.addVar("a");
            Var b = m.addVar("b");
            Var c = m.addVar("c");
            vector<double> x = {2., 3., 0.};
            Solution::SolverStatus s = Solution::SolverStatus::SUCCESS;
            m.getSolution().set(s, 3, 0, x.data());

			Expr e;
			e = 2.*a + b + c;
            TS_ASSERT_EQUALS(7., e.x());

			e = a*b + c;
            TS_ASSERT_EQUALS(6., e.x());

			e = a + b*c;
            TS_ASSERT_EQUALS(2., e.x());

			e = a + b + sin(c);
            TS_ASSERT_EQUALS(5., e.x());
        }

      void testParam(){
         TestModel m;
          Var a = m.addVar("a");
          Param x = m.addParam(2, "x");
          Tes(a+x, "a+[x]", OP_ADD);
          Tes(a*x, "a*[x]", OP_MUL);
      }
};                                      
