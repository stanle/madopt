
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
#include "../src/ipopt_model.hpp"
#include "../src/bonmin_model.hpp"
#include <unistd.h>
#include <cmath>
#include <math.h>
#include <vector>

using namespace MadOpt;

void playground(double a, int b){
    double x = std::pow(-1, 0.5);
    std::cout<<x<<std::endl;
}

void profile(double a, int b){
    int N = std::pow(10, a);

    IpoptModel m;
    Expr obj(0);
    vector<Var> x(N);
    for (int i=0; i<N; i++){
        x[i] = m.addVar(-1.5, 0, -0.5, "x" + to_string(i));
        obj += pow(x[i] - 1, 2);
    }
    m.setObj(obj);

    for (int i=0; i<N-2; i++){
        double a = double(i+2)/(double)N;
        m.addEqConstr((pow(x[i+1], 2) + 1.5*x[i+1] - a)*cos(x[i+2]) - x[i], 0);
    }

    vector<double> xx(N, 0);

    m.init();

    for (int i=0; i<b; i++){
        std::cout<<"run="<<i<<std::endl;
        m.setEvals(xx.data());
    }
}

void test(double d, int i){
    IpoptModel m;
    m.show_solver = true;
    Var b = m.addVar(0, "b");
    m.addEqConstr(cos(b), 0);
    m.setObj(Expr(0));
    m.setStringOption("hessian_approximation", "limited_memory");
    m.solve();
}

void constructModel(const int N, IpoptModel& m, vector<Var>& x){
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
    m.show_solver = true;
}

void tutorial(double p, int i){
    int N = std::pow(10, p);

    IpoptModel m;
//    m.setIntegerOption("print_level", 12);
//    m.setStringOption("print_timing_statistics", "yes");
//    m.setStringOption("hessian_approximation", "limited-memory");

    vector<Var> x(N);

    constructModel(N, m, x);

    m.solve();

    cout<<m.status()<<endl;
    cout<<(m.status()==Solution::SolverStatus::SUCCESS)<< endl;
    cout.precision(30);
    cout<<m.objValue()<<endl;
    cout<<x[0].x()<<endl;
    cout<<x[1].x()<<endl;
    cout<<x[2].x()<<endl;
}

int main(int argc, char* argv[]){
    double d = 5;
    int n = 1;
    size_t func = 0;
    int c;
    while((c = getopt(argc, argv, "n:d:f:")) != -1)
        switch (c){
            case 'f': 
                func = atoi(optarg);
            case 'd':
                d = atoi(optarg);
                break;
            case 'n':
                n = atof(optarg);
                break;
        }

    vector<function<void(double, int)> > funcs 
        = {tutorial, profile, test, playground};

    if (func < funcs.size())
        funcs[func](d, n);
    else
        std::cout<<"invalid input"<<std::endl;

    return 0;
}
