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

class ModelTest: public CxxTest::TestSuite {
    public:

        void testModel(){
            Idx N = pow(10, 3);
           TestModel m;
            for (int i=0; i<N; i++)
                m.addVar(-1.5, 0, -0.5, "x" + to_string(i));
            m.setObj(new TutorialObjTerm(N, 0));
           for (int i=0; i<N-2; i++)
               m.addConstr(new TutorialConstraint(i, (double(i+2)/(double)N)));
           m.init();
        }

        void testModelExp(){
           TestModel m;
            Idx N = pow(10, 3);
            vector<Var> x(N);
            Expr obj(0);
            for (int i=0; i<N; i++){
                x[i] = m.addVar(-1.5, 0, -0.5, "x" + to_string(i));
                obj += pow(x[i] - 1, 2);
            }
            m.setObj(obj);
            for (int i=0; i<N-2; i++){
                double a = double(i+2)/(double)N;
                m.addEqConstr((pow(x[i+1], 2) + 1.5*x[i+1] - a)*cos(x[i+2]) - x[i], 0);
            }

            m.init();
        }

        void testModelBug(){
           TestModel m;
            Idx N = pow(10, 3);
            vector<Var> x(N);
            Expr obj(0);
            for (int i=0; i<N; i++){
                x[i] = m.addVar(-1.5, 0, -0.5, "x" + to_string(i));
                obj += pow(x[i] - 1, 2);
            }
            m.setObj(obj);
            for (int i=0; i<N-2; i++){
                double a = double(i+2)/(double)N;
                m.addEqConstr((pow(x[i+1], 2) + 1.5*x[i+1] - a)*(x[i+2]) - x[i], 0);
            }

            m.init();

            TS_ASSERT_EQUALS(m.nx(), N);
            TS_ASSERT_EQUALS(m.ng(), N-2);

            int njac = m.getNNZ_Jac();
            TS_ASSERT_EQUALS(njac, 3*(N-2))
            vector<int> jac_row(njac);
            vector<int> jac_col(njac);
            m.getNZ_Jac(jac_row.data(), jac_col.data());

            int nhess = m.getNNZ_Hess();
            TS_ASSERT_EQUALS(nhess, N + N-2)
            vector<int> hess_row(nhess);
            vector<int> hess_col(nhess);
            m.getNZ_Hess(hess_row.data(), hess_col.data());

            vector<double> lb(m.nx());
            vector<double> ub(m.nx());
            vector<double> lg(m.ng());
            vector<double> ug(m.ng());
            m.getBounds(lb.data(), ub.data(), lg.data(), ug.data());

            vector<double> xval(m.nx());

            double obj_value;
            vector<double> grad_f(N);
            vector<double> g(N-2);
            vector<double> jac_values(njac);
            vector<double> hess_values(nhess);

            vector<double> lambda(m.ng());

            for (int a=0; a<3; a++){
                for (int i=0; i<N; i++)
                    xval[i] = 2.3*a*i;

                for (int i=0; i<m.ng(); i++)
                    lambda[i] = i*a;

                m.eval_f(xval.data(), true, obj_value);
                m.eval_grad_f(xval.data(), true, grad_f.data());
                m.eval_g(xval.data(), true, g.data());
                m.eval_jac_g(xval.data(), true, jac_values.data());
                m.eval_h(xval.data(), true, hess_values.data(), a, lambda.data());
            }
        }


        void testModelFull(){
           TestModel m;
            Idx N = pow(10, 3);
            vector<Var> x(N);
            Expr obj(0);
            for (int i=0; i<N; i++){
                x[i] = m.addVar(-1.5, 0, -0.5, "x" + to_string(i));
                obj += pow(x[i] - 1, 2);
            }
            m.setObj(obj);
            for (int i=0; i<N-2; i++){
                double a = double(i+2)/(double)N;
                m.addEqConstr((pow(x[i+1], 2) + 1.5*x[i+1] - a)*cos(x[i+2]) - x[i], 0);
            }

            m.init();

            TS_ASSERT_EQUALS(m.nx(), N);
            TS_ASSERT_EQUALS(m.ng(), N-2);

            int njac = m.getNNZ_Jac();
            TS_ASSERT_EQUALS(njac, 3*(N-2))
            vector<int> jac_row(njac);
            vector<int> jac_col(njac);
            m.getNZ_Jac(jac_row.data(), jac_col.data());

            int nhess = m.getNNZ_Hess();
            TS_ASSERT_EQUALS(nhess, N + N-2)
            vector<int> hess_row(nhess);
            vector<int> hess_col(nhess);
            m.getNZ_Hess(hess_row.data(), hess_col.data());

            vector<double> lb(m.nx());
            vector<double> ub(m.nx());
            vector<double> lg(m.ng());
            vector<double> ug(m.ng());
            m.getBounds(lb.data(), ub.data(), lg.data(), ug.data());

            vector<double> xval(m.nx());

            double obj_value;
            vector<double> grad_f(N);
            vector<double> g(N-2);
            vector<double> jac_values(njac);
            vector<double> hess_values(nhess);

            vector<double> lambda(m.ng());

            for (int a=0; a<3; a++){
                for (int i=0; i<N; i++)
                    xval[i] = 2.3*a*i;

                for (int i=0; i<m.ng(); i++)
                    lambda[i] = i*a;

                m.eval_f(xval.data(), true, obj_value);
                m.eval_grad_f(xval.data(), true, grad_f.data());
                m.eval_g(xval.data(), true, g.data());
                m.eval_jac_g(xval.data(), true, jac_values.data());
                m.eval_h(xval.data(), true, hess_values.data(), a, lambda.data());
            }
        }

        void testSolution(){
           TestModel m;
            //TS_ASSERT_THROWS(m.status(), MadOptError);
            TS_ASSERT_THROWS(m.objValue(), MadOptError);

            vector<double> x = {3};
            m.getSolution().set(Solution::SolverStatus::SUCCESS, 1, 2, x.data());
            TS_ASSERT_EQUALS(m.status(), Solution::SolverStatus::SUCCESS);
            TS_ASSERT_EQUALS(m.objValue(), 2);
            TS_ASSERT_EQUALS(m.getSolution().x(0), 3);
            TS_ASSERT_THROWS(m.getSolution().lam(0), MadOptError);

            m.getSolution().set(Solution::SolverStatus::LOCAL_INFEASIBILITY, 1, 2, x.data());
            TS_ASSERT_EQUALS(m.status(), Solution::SolverStatus::LOCAL_INFEASIBILITY);
            TS_ASSERT_THROWS(m.objValue(), MadOptError);
            TS_ASSERT_THROWS(m.getSolution().x(0), MadOptError);
        }

        void testgetJac(){
            TestModel m;

            Var a = m.addVar("a");
            Var b = m.addVar("b");
            Var c = m.addVar("c");
            m.addConstr(1, a+b, 3);
            m.addConstr(1, a-c, 3);
            m.setObj(a);

            m.init();

            TS_ASSERT_EQUALS(m.getNNZ_Jac(), 4);
            TS_ASSERT_EQUALS(m.getNNZ_Hess(), 0);

            vector<int> iRow(4);
            vector<int> jCol(4);
            m.getNZ_Jac(iRow.data(), jCol.data());

            vector<int> iRow_res = {0, 0, 1, 1};
            vector<int> jCol_res = {0, 1, 0, 2};

            TS_ASSERT_EQUALS(iRow, iRow_res);
            TS_ASSERT_EQUALS(iRow, iRow_res);
        }
};
