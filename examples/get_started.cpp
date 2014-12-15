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
#include <madopt/ipopt_model.hpp>
//#include <madopt/bonmin_model.hpp>
#include <vector>
using namespace std;

int main(){
    int N = pow(10, 4);

    //create IpoptModel
    MadOpt::IpoptModel m;

    //create BonminModel
    //MadOpt::BonminModel m;
    
    // Options
    //
    //
    //m.setIntegerOption("print_level", 5);
    //m.setStringOption("print_timing_statistics", "yes");
    //m.setStringOption("hessian_approximation", "limited-memory");

    // shortcut flags
    // set flag if solver output should be printed
    m.show_solver = true;
    // timelimit, a negative value disables the time limit
    m.timelimit = 12;

    // Variables
    //
    //
    // create continuous variable x
    // lower bound = -1.5
    // upper bound = 0
    // initial value = -0.5
    // MadOpt::Var x = m.addVar(-1.5, 0, -0.5, "x");

    // to set a bound to infinity use MadOpt::INF
    // MadOpt::Var v = m.addVar(MadOpt::INF, 0, -0.5, "v");

    // create integer variable y
    // lower bound = 1
    // upper bound = 10
    // initial value = 4
    // MadOpt::Var y = m.addIVar(1, 10, 4, "y");

    // create binary variable z
    // initial value = 1
    // MadOpt::Var z = m.addBVar(1, "z");

    vector<MadOpt::Var> x(N);
    for (int i=0; i<N; i++){
        x[i] = m.addVar(-1.5, 0, -0.5, "x" + to_string(i));
    }

    // Parameters
    //
    //
    // create Parameter
    // value = 1
    // name = p
    // MadOpt::Param p = m.addParam(1, "p");
    vector<MadOpt::Param> p(N-2);
    for (int i=0; i<N-2; i++){
        double a = double(i+2)/(double)N;
        p[i] = m.addParam(a, "p"+to_string(i));
    }

    // Constraints
    //
    //
    // expr = expression build from constants, variables MadOpt::Var, parameters MadOpt::Param
    // lb = lower bound, double
    // ub = upper bound, double
    //
    // add constraint of type:
    // 
    // lb <= expr <= ub
    // m.addConstr(lb, expr, ub);
    //
    // lb <= expr <= INF
    // m.addConstr(lb, expr);
    //
    // INF <= expr <= ub
    // m.addConstr(expr, ub);
    //
    // expr == eq
    // m.addEqConstr(expr, eq);

    vector<MadOpt::Constraint> c(N-2);
    for (int i=0; i<N-2; i++){
        c[i] = m.addEqConstr((MadOpt::pow(x[i+1], 2) + 1.5*x[i+1] - p[i])*MadOpt::cos(x[i+2]) - x[i], 0);
    }

    // Objective
    //
    //
    MadOpt::Expr obj(0);
    for (int i=0; i<N; i++)
        obj += MadOpt::pow(x[i] - 1, 2);

    // set objective
    m.setObj(obj);

    // calling the solver
    //
    //
    m.solve();

    // check if a solution exists, or in other words if the solver succeeded
    //
    //
    if (m.hasSolution()){

        // access objective value
        cout<<"Objective:"<<m.objValue()<<endl;

        // access solver status == Ipopt status values
        cout<<"Solver status:"<<m.status()<<endl;

        //print solution value of first variable
        cout<<"Solution value for x0:"<<x[0].x()<<endl;

        //print lambda value of constraint, only available for IpoptModel
        cout<<"Lambda value of constraint 0:"<<c[0].lam()<<endl;
    }

    //changing the model
    //
    //
    // Variables
    //change variable bounds and initial value
    x[1].lb(-12);
    x[1].ub(1);
    x[1].init(0);
    //access bounds and initial values
    cout<<"Variable values: lb="<<x[1].lb()<<" ub="<<x[1].ub()<<" init="<<x[1].init()<<endl;

    //Constraints
    //change constraint bounds
    c[0].lb(-1);
    c[0].ub(2);
    //access bounds 
    cout<<"Constraint bounds: lb="<<c[0].lb()<<" ub="<<c[0].ub()<<endl;

    //Parameter
    // change value
    p[0].value(12);
    // access value
    cout<<"Parameter value:"<<p[0].value()<<endl;

    // resolve
    //
    //
    // set solution as initial values
    m.solAsInit();

    m.show_solver = false;
    m.solve();
    if (not m.hasSolution())
        cout<<"no solution"<<endl;
    // test for specific solver status
    if (m.status()==MadOpt::Solution::LOCAL_INFEASIBILITY)
        cout<<"Infeasible"<<endl;

    return 0;
}
