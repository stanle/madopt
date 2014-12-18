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
#include "model.hpp"

#include "common.hpp"
#include "inner_var.hpp"
#include "var.hpp"
#include "inner_param.hpp"
#include "param.hpp"
#include "inner_constraint.hpp"
#include "constraint.hpp"
#include "econstraint.hpp"
#include "logger.hpp"

using namespace MadOpt;

Model::~Model(){
    for (auto p: vars)
        delete p;

    for (auto p: params)
        delete p;

    for (auto p: constraints)
        delete p;

    if (obj_tmp != nullptr)
        delete[] obj_tmp;

    if (obj != nullptr){
        delete obj;
    }
}

void Model::init(){
    TRACE_START;
    if (show_solver)
        cout<<"Starting Init"<<endl;

    hess_pos_map.clear();

    if (obj == nullptr)
        throw MadOptError("no objective set");

    obj->init(hess_pos_map);
    for (auto constr: constraints){
        constr->init(hess_pos_map);
    }

    stack.clear();

    stack.optimizeAlignment();

    obj_jac_map = obj->getJacEntries();

    if (obj_tmp != nullptr)
        delete[] obj_tmp;
    obj_tmp = new double[obj_jac_map.size()];

    if (show_solver)
        cout<<"init end"<<endl;
    TRACE_END;
}

// Var stuff
// 
//
Var Model::addVar(string name){
    return addCVar(-INF, INF, 0, name);
}

Var Model::addVar(double lb, double ub, double init, string name){
   return addCVar(lb, ub, init, name);
}

Var Model::addVar(double lb, double ub, string name){
    return addCVar(lb, ub, name);
}

Var Model::addCVar(double lb, double ub, double init, string name){
    return addVar(lb, ub, VarType::CONTINUOUS, init, name);
}

Var Model::addCVar(double lb, double ub, string name){
    double init = max(min(0.0, ub), lb);
    return addCVar(lb, ub, init, name);
}

Var Model::addIVar(double lb, double ub, double init, string name){
    return addVar(lb, ub, VarType::INTEGER, init, name);
}

Var Model::addIVar(double lb, double ub, string name){
    double init = max(min(0.0, ub), lb);
    return addIVar(lb, ub, init, name);
}

Var Model::addBVar(double init, string name){
    return addVar(0, 1, VarType::BINARY, init, name);
}

//Constraint stuff
//
//
Constraint Model::addConstr(InnerConstraint* con){
    con->setPos(constraints.size());
    con->setSolutionClass(&solution);
    constraints.push_back(con);
    model_changed = true;
    return Constraint(con);
}

Constraint Model::addConstr(const double lb, const Expr& expr, const double ub){
    if (lb > ub)
        throw MadOptError("lower bound is greater then upper bound for expr=" 
                + expr.toString() 
                + " lb=" + std::to_string(lb) 
                + " ub=" + std::to_string(ub));
    auto vars = expr.getInnerVariables();
    for (auto var: vars){
        const Solution& sol = var->getSolution();
        if (&solution != &sol)
            throw MadOptError("added variable from other model to wrong model");
    }
    return addConstr(new EConstraint(expr, lb, ub, stack));
}

Constraint Model::addEqConstr(const Expr& expr, const double equal){
    return addConstr(equal, expr, equal);
}

Constraint Model::addConstr(const Expr& expr, const double ub){
    return addConstr(-INF, expr, ub);
}

Constraint Model::addConstr(const double lb, const Expr& expr){
    return addConstr(lb, expr, INF);
}

//Objective Stuff
//
//
void Model::setObj(InnerConstraint* constraint){
    model_changed = true;
    if (obj != nullptr)
        delete obj;
    obj = constraint;
}

void Model::setObj(const Expr& expr){
    setObj(new EConstraint(expr, stack));
}

//NLP init stuff
//
//
Idx Model::getNNZ_Jac(){
    int res = 0;
    for (auto constr: constraints){
        res += constr->getNNZ_Jac();
    }
    VALGRIND_CONDITIONAL_JUMP_TEST(res);
    return res;
}

Idx Model::getNNZ_Hess(){
    return hess_pos_map.size();
}

void Model::getNZ_Jac(int* iRow, int* jCol){
    TRACE_START;
    int nz = 0;
    int pos = 0;
    for (auto constr: constraints){
        Idx size = constr->getNNZ_Jac();
        for (Idx i=0; i<size; i++){
            iRow[nz + i] = pos;
        }
        pos++;

        unsigned int* p = (unsigned int*)(&(jCol[nz]));
        constr->getNZ_Jac(p);
        nz += size;
    }
    TRACE_END;
}

void Model::getNZ_Hess(int* iRow, int* jCol){
    for (auto it: hess_pos_map){
        iRow[it.second] = it.first.first;
        jCol[it.second] = it.first.second;
    }
}

void Model::getBounds(double* xl, double* xu, double* gl, double* gu){
    for (Idx i=0; i<nx(); i++){
        VALGRIND_CONDITIONAL_JUMP_TEST(vars[i]->lb());
        VALGRIND_CONDITIONAL_JUMP_TEST(vars[i]->ub());
        xl[i] = vars[i]->lb();
        xu[i] = vars[i]->ub();
    }

    for (Idx i=0; i<ng(); i++){
        VALGRIND_CONDITIONAL_JUMP_TEST(constraints[i]->lb());
        VALGRIND_CONDITIONAL_JUMP_TEST(constraints[i]->ub());
        gl[i] = constraints[i]->lb();
        gu[i] = constraints[i]->ub();
    }
}

void Model::getInits(double* xi){
    for (Idx i=0; i<nx(); i++){
        VALGRIND_CONDITIONAL_JUMP_TEST(vars[i]->init());
        xi[i] = vars[i]->init();
    }
}

void Model::solAsInit(){
    for (auto var: vars)
        var->solAsInit();
}

Idx Model::nx() const{
    return vars.size();
}

Idx Model::ng() const{
    return constraints.size();
}

Idx Model::np() const{
    return params.size();
}

// Eval functions
// 
// 
void Model::setEvals(const double* x){
    obj->setEvals(x);
    for (auto constraint: constraints)
        constraint->setEvals(x);
}

void Model::eval_f(const double* x, bool new_x, double& obj_value){
    if (new_x) setEvals(x);
    obj_value = obj->eval(x);
    VALGRIND_CONDITIONAL_JUMP_TEST(obj_value);
}

void Model::eval_grad_f(const double* x, bool new_x, double* grad_f){
    if (new_x) setEvals(x);
    for (Idx i=0; i<nx(); i++)
        grad_f[i] = 0;

    obj->eval_jac(x, obj_tmp);
    for (Idx i=0; i<obj_jac_map.size(); i++){
        VALGRIND_CONDITIONAL_JUMP_TEST(obj_tmp[i]);
        grad_f[obj_jac_map[i]] = obj_tmp[i];
    }
}

void Model::eval_g(const double* x, bool new_x, double* g){
    if (new_x) setEvals(x);
    for (Idx i=0; i<ng(); i++){
        g[i] = constraints[i]->eval(x);
        VALGRIND_CONDITIONAL_JUMP_TEST(g[i]);
    }
}

void Model::eval_jac_g(const double* x, bool new_x, double* values){
    if (new_x) setEvals(x);
    int nz = 0;
    for (auto constraint: constraints){
        constraint->eval_jac(x, &values[nz]);
        nz += constraint->getNNZ_Jac();
    }
}

void Model::eval_h(const double* x, bool new_x, double* values, double obj_factor, const double* lambda){
    if (new_x) setEvals(x);
    for (Idx i=0; i<hess_pos_map.size(); i++)
        values[i] = 0;
    for (Idx i=0; i<ng(); i++)
        constraints[i]->eval_h(x, values, lambda[i]);
    obj->eval_h(x, values, obj_factor);
}

double Model::objValue()const { 
    return solution.obj_value(); 
}

Solution::SolverStatus Model::status()const {
    return solution.status(); 
}

Var Model::addVar(double lb, double ub, VarType type, double init, string name){
    InnerVar* v = new InnerVar(lb, ub, init, type, name, solution);
    v->setPos(vars.size());
    vars.push_back(v);
    model_changed = true;
    return Var(v);
}

Param Model::addParam(const double value, const string name){
    InnerParam* p = new InnerParam(value, name);
    params.push_back(p);
    return Param(p);
}

bool Model::hasSolution() const{
    return solution.hasSolution();
}

const string Model::toString()const {
    string res;
    for (auto var : vars)
        res += var->toString() + "\n";

//    for (auto constraint: constraints)
//        res += constraint->toString() + "\n";

    return res;

}
/* ex: set tabstop=4 shiftwidth=4 expandtab: */
