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
#include "../src/inner_constraint.hpp"

using namespace MadOpt;

class EConstraintTest: public CxxTest::TestSuite {
    public:

        void Tes(Expr exp, 
                const vector<double> x,
                const double g,
                const vector<Idx> expected_jac_entries, 
                const vector<double> expected_jac_values,
                const vector<PII> expected_hess_entries={}, 
                const vector<double> expected_hess_values={}, 
                const double delta=0.000001
                ){
            HessPosMap hess_pos_map;
            TestModel m;
            auto& simstack = m.getSimStack();
            simstack.setXSize(x.size());
            InnerConstraint constraint(exp, 0, 0, hess_pos_map, simstack);
            
            auto& cstack = m.getCStack();
            cstack.resize(simstack);
            cstack.setX(x.data());
            constraint.setEvals(cstack);

            map<int, double> expected_jac_map;
            for (Idx i=0; i<expected_jac_entries.size(); i++)
                expected_jac_map[expected_jac_entries[i]] = expected_jac_values[i];
	    const auto& real_jac_entries = constraint.getJacEntries();
            map<int, double> real_jac_map;
            auto real_jac_values = constraint.getJac();
            for (Idx i=0; i<constraint.getNNZ_Jac(); i++)
                real_jac_map[real_jac_entries[i]] = real_jac_values[i];
            
	    map<PII, double> expected_hess_map;
            for (Idx i=0; i<expected_hess_entries.size(); i++)
                expected_hess_map[expected_hess_entries[i]] = expected_hess_values[i];
	    const auto& real_hess_entries_as_index = constraint.getHessMap();
            vector<double> real_hess_values(hess_pos_map.size(), 0);
            auto real_hess_values_for_index = constraint.getHess();
            int i=0;
            for (auto id: real_hess_entries_as_index)
                real_hess_values[id] += real_hess_values_for_index[i++];
            map<PII, double> real_hess_map;
            for (auto p: hess_pos_map)
                real_hess_map[p.first] = real_hess_values[p.second];
            
            TS_ASSERT_DELTA(constraint.getG(), g, delta);

            TS_ASSERT_EQUALS(real_jac_entries.size(), real_jac_values.size());
            TS_ASSERT_EQUALS(real_jac_entries.size(), expected_jac_entries.size());
            TS_ASSERT_EQUALS(real_jac_values.size(), expected_jac_values.size());

            TS_ASSERT_EQUALS(real_hess_entries_as_index.size(), real_hess_values_for_index.size());
            TS_ASSERT_EQUALS(real_hess_entries_as_index.size(), expected_hess_entries.size());
            TS_ASSERT_EQUALS(real_hess_values_for_index.size(), expected_hess_values.size());
	    TS_ASSERT_EQUALS(hess_pos_map.size(), expected_hess_entries.size());

            if (delta == 0){
                TS_ASSERT_EQUALS(real_jac_map, expected_jac_map);
                TS_ASSERT_EQUALS(real_hess_map, expected_hess_map);
            } else {

                for (auto p: real_jac_map)
                    TS_ASSERT_DELTA(p.second, expected_jac_map.at(p.first), delta);

                for (auto p: real_hess_map)
                    TS_ASSERT_DELTA(p.second, expected_hess_map.at(p.first), delta);
            }
        }

        void testVar(){
            TestModel m;
            Var a = m.addVar("a");
            Tes(a, {3}, 3, {0}, {1});
        }

        void testParam(){
            TestModel m;
            Var a = m.addVar("a");
            Param x = m.addParam(3, "x");
            Tes(x*a, {2}, 6, {0}, {3});
            x.value(4);
            Tes(x*a, {2}, 8, {0}, {4});
        }

        void testADD(){
            TestModel m;
            Var a = m.addVar("a");
            Var b = m.addVar("b");
            Var c = m.addVar("c");
            Tes(a+b, {2, 3}, 2+3, {0, 1}, {1,1});
            Tes(a+b+c, {2, 3, 4}, 2+3+4, {0, 1, 2}, {1,1,1});
        }

        void testMUL(){
            TestModel m;
            Var a = m.addVar("a");
            Var b = m.addVar("b");
            //Tes(2*a, {3}, 6, {0}, {2});
            //Tes(a*b, {2,3}, 2*3, {0,1}, {3,2}, {PII(0,1)}, {1});
            double ax = 3;
            double bx = 5;
            Tes(a*a*b, {ax,bx}, 
                ax*ax*bx, 
                {0,1}, {2*ax*bx, ax*ax},
                {PII(0,0), PII(0,1)}, {2*bx, 2*ax});
        }

        void testCOS(){
            TestModel m;
            Var a = m.addVar("a");
            Tes(cos(2*a), {3}, cos(2*3), {0}, {-2*sin(6)}, {PII(0,0)}, {-4*cos(6)});
        }

  void testLN(){
    TestModel m;
    Var a = m.addVar("a");
    Tes(ln(2*a), {3}, std::log(2*3), {0}, {2.0/6.0}, {PII(0,0)}, {-1.0/9.0});
  }

  void testLOG2(){
    TestModel m;
    Var a = m.addVar("a");
    Tes(log2(2*a), {3}, std::log2(2*3), {0}, {2.0/(6*log(2))}, {PII(0,0)}, {-2.0*2.0*log(2)/pow(2*3*log(2), 2)});
  }

        void testCOS2(){
            TestModel m;
            Var a = m.addVar("a");
            Var b = m.addVar("b");
            Tes(cos(a*b), {3, 2}, cos(2*3), 
                    {0, 1}, {-2*sin(6), -3*sin(6)},
                    {PII(0,0), PII(0,1), PII(1,1)}, 
                    {-4*cos(6), -sin(6)-6*cos(6), -9*cos(6)});
        }

        void testSIN(){
            TestModel m;
            Var a = m.addVar("a");
            Tes(sin(2*a), {3}, sin(2*3), {0}, {2*cos(6)}, {PII(0,0)}, {-4*sin(6)});
        }

        void testPOW(){
            TestModel m;
            Var a = m.addVar("a");
            Tes(pow(a,2), {3}, pow(3,2), {0}, {2*3}, {PII(0,0)}, {2});
            Tes(pow(2*a, 3), {3}, pow(6,3), {0}, {3*2*pow(2*3,2)}, {PII(0,0)}, {3*2*2*2*pow(2*3,1)});
        }

        void testMulti(){
            TestModel m;
            Var a = m.addVar("a");
            Var b = m.addVar("b");
            Var c = m.addVar("c");
            Tes(2*a+b, {2, 3}, 2*2+3, {0,1}, {2,1});
            Tes(c*a+b, {2,3,4}, 4*2+3, {0,1,2}, {4,1,2}, {PII(0,2)}, {1});
            double ax = 3;
            double bx = 2;
            Tes((2*a+1.5)*b, {ax, bx}, (2*ax+1.5)*bx,
                {0,1}, {2*bx, 2*ax+1.5}, {PII(0,1)}, {2});
        }

        void testBug(){
            TestModel m;
            Var a = m.addVar("a");
            Var b = m.addVar("b");

            double ax = 3;
            double bx = 2;
            double v = 3.0/100.0;

            Tes((a*a + 1.5*a - v)*b, {ax,bx}, 
                (ax*ax + 1.5*ax-v)*bx, 
                {0,1}, {(2*ax+1.5)*bx, ax*ax+1.5*ax-v},
                {PII(0,0), PII(0,1)}, {2*bx, 2*ax+1.5});
        }

        void testTutorialTerm(){
            TestModel m;
            Var a = m.addVar("a");
            Var b = m.addVar("b");
            Var c = m.addVar("c");

            double ax = 3.1;
            double bx = 2;
            double cx = 1;
            double v = 3.0/100;

            Tes((pow(a,2) + 1.5*a - v)*cos(b)-c, {ax,bx,cx}, 
                (pow(ax,2)+1.5*ax-v)*cos(bx)-cx, 
                {0,1,2}, {(2*ax+1.5)*cos(bx), -(pow(ax,2)+1.5*ax-v)*sin(bx), -1},
                {PII(0,0), PII(0,1), PII(1,1)},
                {cos(bx)*2, -sin(bx)*(2*ax+1.5), -(pow(ax,2)+1.5*ax-v)*cos(bx)});
        }

        void testCaseADD(){
            for (int N=1; N<13; N++){
                TestModel m;
                Expr term(0);
                vector<double> x(N);
                vector<Idx> jac_entries(N);
                vector<double> jac(N);
                vector<PII> hess_entries(N);
                vector<double> hess(N);
                double g = 0;
                for (int i=0; i < N; ++i) {
                    term += pow(m.addVar("x"), 2);
                    x[i] = i;
                    g += i*i;
                    jac_entries[i] = i;
                    jac[i] = 2*i;
                    hess_entries[i] = PII(i,i);
                    hess[i] = 2;
                }

                Tes(term, x, g, jac_entries, jac, hess_entries, hess);
            }
        }

        void testLargeSum(){
            int N = 4;
            TestModel m;
            Expr term(0);
            vector<double> x(N);
            vector<Idx> jac_entries(N);
            vector<double> jac(N);
            vector<PII> hess_entries(N);
            vector<double> hess(N);
            double g = 0;
            for (int i=0; i < N; ++i) {
                term += pow(m.addVar("x" + std::to_string(i)), 2);
                x[i] = i;
                g += i*i;
                jac_entries[i] = i;
                jac[i] = 2*i;
                hess_entries[i] = PII(i,i);
                hess[i] = 2;
            }

            Tes(term, x, g, jac_entries, jac, hess_entries, hess);
        }

        void testLargeSum2(){
            int N = 4;
            TestModel m;
            vector<Var> vars(N);
            vector<double> x(N);
            vector<Idx> jac_entries(N);
            vector<double> jac(N);
            vector<PII> hess_entries(2*(N-1)+1);
            vector<double> hess(2*(N-1)+1);

            for (int i=0; i < N; ++i) {
                vars[i] = m.addVar("x");
                x[i] = i;
                jac_entries[i] = i;
            }

            Expr term(0);
            double g = 0;
            Idx hess_counter = 0;
            for (int i=0; i < N-1; ++i) {
                term += pow(vars[i] + vars[i+1], 2);
                g += (2*i+1)*(2*i+1);
            }

            hess_entries[hess_counter] = PII(0,0);
            hess[hess_counter++] = 2;
            hess_entries[hess_counter] = PII(0,1);
            hess[hess_counter++] = 2;
            jac[0] = 2*(0 + 1);
            for (int i=1; i < N-1; ++i) {
                jac[i] = 2*(i+i+1) + 2*(i-1+i);
                hess_entries[hess_counter] = PII(i,i);
                hess[hess_counter++] = 2+2;
                hess_entries[hess_counter] = PII(i,i+1);
                hess[hess_counter++] = 2;
            }
            jac[N-1] = 2*(N-2 + N-1);
            hess_entries[hess_counter] = PII(N-1,N-1);
            hess[hess_counter++] = 2;

            Tes(term, x, g, jac_entries, jac, hess_entries, hess);
        }
        
	void testXimesXTimesX(){
            TestModel m;
            Var x = m.addVar("x");
            Tes(x*x*x, {1}, 1, {0}, {3}, {PII(0,0)}, {6});
        }

	void testIssue25Bug(){
	    TestModel m;
	    auto x = m.addVar(0.999, 1.001, 1.0, "x");
	    auto e = x * pow(pow(x, 2) + x, -1); 
	    Tes(e, {1}, 1 * pow(pow(1, 2) + 1, -1), {0}, {-0.25}, {PII(0,0)}, {0.25});
	}

	void testIssue25Bug2(){
	    TestModel m;
	    auto x = m.addVar(0.999, 1.001, 1.0, "x");
	    auto e = pow(pow(x, 2) + x, -1) * x; 
	    Tes(e, {1}, 1 * pow(pow(1, 2) + 1, -1), {0}, {-0.25}, {PII(0,0)}, {0.25});
	}
};

