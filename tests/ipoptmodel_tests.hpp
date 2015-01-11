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
#include "../src/ipopt_model.hpp"
#include "../src/exceptions.hpp"

using namespace MadOpt;

class IpoptModelTest: public CxxTest::TestSuite {
    public:

        void testSimple(){
            IpoptModel m;
            Var a = m.addVar(0, 1, 0.5, "a");
            m.addConstr(1, a, 3);
            m.setObj(a);
            m.solve();

            TS_ASSERT_DELTA(a.x(), 1.0, 0.0001);
            TS_ASSERT_EQUALS(m.status(), 0.0);
            TS_ASSERT_DELTA(m.objValue(), 1.0, 0.0001);
        }

        void testSimple2(){
            IpoptModel m;
            Var a = m.addVar(0, 1, 0.5, "a");
            Var b = m.addVar(0, 1, 0.5, "b");

            m.addConstr(1, a+b, 3);
            m.addConstr(0, b-a, 3);
            m.setObj(a);

            m.solve();

            TS_ASSERT_EQUALS(a.x(), 0.0);
            TS_ASSERT_EQUALS(b.x(), 1.0);
            TS_ASSERT_EQUALS(m.status(), 0.0);
            TS_ASSERT_DELTA(m.objValue(), 0.0, 0.00001);
        }

        void testTutorial(){
            Idx N = pow(10, 3);
            IpoptModel m;
            vector<Var> x(N);
            Expr obj(0);
            for (Idx i=0; i<N; i++){
                x[i] = m.addVar(-1.5, 0, -0.5, "x" + std::to_string(i));
                obj += pow(x[i] - 1, 2);
            }

            m.setObj(obj);

            for (Idx i=0; i<N-2; i++){
                double a = double(i+2)/(double)N;
                m.addEqConstr((pow(x[i+1], 2) + 1.5*x[i+1] - a)*cos(x[i+2]) - x[i], 0);
            }

            m.solve();

            TS_ASSERT_EQUALS(m.status(), 0);
            TS_ASSERT_EQUALS(m.objValue(),  3013.9151946976758154050912708);
            TS_ASSERT_EQUALS(x[0].x(), -0.413735377524265646176360178288);
            TS_ASSERT_EQUALS(x[1].x(), -0.415322942791292726294472004156);
            TS_ASSERT_EQUALS(x[2].x(), -0.416896632962390045751277511954);
        }

        void testResolve(){
            IpoptModel m;
            Var a = m.addVar(0, 1, 0.5, "a");
            Var b = m.addVar(0, 1, 0.5, "b");

            m.addConstr(1, a+b, 3);
            m.setObj(a);

            m.solve();

            a.ub(2);
            a.init(1.5);
            a.lb(1);

            m.solve();

            TS_ASSERT_EQUALS(a.x(), 1.0);
            TS_ASSERT_EQUALS(m.status(), 0.0);
            TS_ASSERT_DELTA(m.objValue(), 1.0, 0.00001);
        }

        void testNoSolution(){
            IpoptModel m;
            Var a = m.addVar(0, 1, 0.5, "a");
            m.addConstr(2, a, 3);
            m.setObj(a);
            m.solve();

            TS_ASSERT_EQUALS(m.status(), MadOpt::Solution::LOCAL_INFEASIBILITY);
            TS_ASSERT_THROWS(m.objValue(), MadOptError);
            TS_ASSERT_THROWS(a.x(), MadOptError);
        }

        void testResolveTutorial(){
            int N = pow(10, 3);
            MadOpt::IpoptModel m;
            vector<MadOpt::Var> x(N);
            for (int i=0; i<N; i++){
                x[i] = m.addVar(-1.5, 0, -0.5, "x" + to_string(i));
            }

            vector<MadOpt::Param> p(N-2);
            for (int i=0; i<N-2; i++){
                double a = double(i+2)/(double)N;
                p[i] = m.addParam(a, "p"+to_string(i));
            }

            vector<MadOpt::Constraint> c(N-2);
            for (int i=0; i<N-2; i++){
                c[i] = m.addEqConstr((MadOpt::pow(x[i+1], 2) + 1.5*x[i+1] - p[i])*MadOpt::cos(x[i+2]) - x[i], 0);
            }

            MadOpt::Expr obj(0);
            for (int i=0; i<N; i++)
                obj += MadOpt::pow(x[i] - 1, 2);

            m.setObj(obj);

            m.solve();

            x[1].lb(-12);
            x[1].ub(1);
            x[1].init(0);
            c[0].lb(-1);
            c[0].ub(2);
            p[0].value(12);
            m.solAsInit();

            m.solve();

            TS_ASSERT_EQUALS(m.status(), MadOpt::Solution::LOCAL_INFEASIBILITY);
            TS_ASSERT_THROWS(m.objValue(), MadOptError);
            TS_ASSERT_THROWS(x[0].x(), MadOptError);
        }

       void testAddConstr(){
           IpoptModel m;
           Var x = m.addVar(0, 100, 1, "x");
           TS_ASSERT_THROWS(m.addConstr(34, x, 12), MadOptError);
       }

       void testINFBounds(){
            IpoptModel m;
            Var y = m.addVar(-INF, INF, 5, "x");
            Var x1 = m.addVar(-INF, INF, "x1");

            m.addEqConstr(y - x1, 0);
            m.addEqConstr(y*y - x1, 0);

            m.setObj(Expr(0));
            m.solve();
            TS_ASSERT(m.hasSolution());
       }
};
