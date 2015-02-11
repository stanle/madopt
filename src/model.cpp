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
#include "logger.hpp"

using namespace MadOpt;

Model::~Model(){
    for (auto& p: vars)
        delete p;

    for (auto& p: params)
        delete p;

    for (auto& p: constraints)
        delete p;

    if (obj != nullptr){
        delete obj;
    }
}

// Var stuff
// 
//
Var Model::addVar(string name){
    return addCVar(-INF, INF, 0, name);
}

Var Model::addVar(double init, string name){
    return addCVar(-INF, INF, init, name);
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
Constraint Model::addConstr(const double lb, const Expr& expr, const double ub){
    TRACE_START;
    if (lb > ub)
        throw MadOptError("lower bound is greater then upper bound for expr=" 
                + expr.toString() 
                + " lb=" + std::to_string(lb) 
                + " ub=" + std::to_string(ub));
    auto vars = expr.getInnerVariables();
    for (auto& var: vars){
        const Solution& sol = var->getSolution();
        if (&solution != &sol)
            throw MadOptError("cannot add variable from other model to this model");
    }
    TRACE(expr.toString());
    simstack.setXSize(nx());
    auto con = new InnerConstraint(expr, lb, ub, hess_pos_map, simstack);
    cstack.resize(simstack);
    constraints.push_back(con);
    model_changed = true;
    TRACE_END;
    return Constraint(this, constraints.size()-1);
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
void Model::setObj(const Expr& expr){
    model_changed = true;
    if (obj != nullptr)
        delete obj;
    simstack.setXSize(nx());
    obj = new InnerConstraint(expr, hess_pos_map, simstack);
    cstack.resize(simstack);
    obj_jac_map.clear();
    obj_jac_map.resize(obj->getNNZ_Jac());
    obj->getNZ_Jac(obj_jac_map.data());
}

//NLP init stuff
//
//
Idx Model::getNNZ_Jac(){
    int res = 0;
    for (auto& constr: constraints){
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
    for (auto& constr: constraints){
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
    for (auto& it: hess_pos_map){
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
    for (auto& var: vars)
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
    cstack.setX(x);
    obj->setEvals(cstack);
    for (auto& constraint: constraints)
        constraint->setEvals(cstack);
}

void Model::eval_f(const double* x, bool new_x, double& obj_value){
    if (new_x) setEvals(x);
    obj_value = obj->getG();
    VALGRIND_CONDITIONAL_JUMP_TEST(obj_value);
}

void Model::eval_grad_f(const double* x, bool new_x, double* grad_f){
    if (new_x) setEvals(x);
    for (Idx i=0; i<nx(); i++)
        grad_f[i] = 0;

    Idx i=0;
    for (auto& value: obj->getJac()){
        VALGRIND_CONDITIONAL_JUMP_TEST(value);
        grad_f[obj_jac_map[i++]] = value;
    }
}

void Model::eval_g(const double* x, bool new_x, double* g){
    if (new_x) setEvals(x);
    for (Idx i=0; i<ng(); i++){
        g[i] = constraints[i]->getG();
        VALGRIND_CONDITIONAL_JUMP_TEST(g[i]);
    }
}

void Model::eval_jac_g(const double* x, bool new_x, double* values){
    if (new_x) setEvals(x);
    int nz = 0;
    for (auto& constraint: constraints){
        auto& jac = constraint->getJac();
        std::copy(jac.begin(), jac.end(), &(values[nz]));
        nz += jac.size();
    }
}

void Model::eval_h(const double* x, bool new_x, double* values, double obj_factor, const double* lambda){
    if (new_x) setEvals(x);

    for (Idx i=0; i<hess_pos_map.size(); i++)
        values[i] = 0;

    for (Idx i=0; i<ng(); i++)
        constraints[i]->eval_h(values, lambda[i]);

    obj->eval_h(values, obj_factor);
}

double Model::objValue()const { 
    return solution.obj_value(); 
}

Solution::SolverStatus Model::status()const {
    return solution.status(); 
}

Var Model::addVar(double lb, double ub, VarType type, double init, string name){
    TRACE_START;
    InnerVar* v = new InnerVar(lb, ub, init, type, name, solution);
    v->setPos(vars.size());
    vars.push_back(v);
    model_changed = true;
    TRACE_END;
    return Var(v);
}

Param Model::addParam(const double value, const string name){
    TRACE_START;
    InnerParam* p = new InnerParam(value, name);
    params.push_back(p);
    TRACE_END;
    return Param(p);
}

bool Model::hasSolution() const{
    return solution.hasSolution();
}

const string Model::toString()const {
    string res;
    for (auto& var : vars)
        res += var->toString() + "\n";

//    for (auto constraint: constraints)
//        res += constraint->toString() + "\n";

    return res;

}

double Model::lb(Idx idx)const {
    ASSERT_LE(idx, constraints.size()-1);
    return constraints[idx]->lb();
}

void Model::lb(Idx idx, double v){
    ASSERT_LE(idx, constraints.size()-1);
    constraints[idx]->lb(v);
}

double Model::ub(Idx idx)const {
    ASSERT_LE(idx, constraints.size()-1);
    return constraints[idx]->ub();
}

void Model::ub(Idx idx, double v){
    ASSERT_LE(idx, constraints.size()-1);
    constraints[idx]->ub(v);
}


/* ex: set tabstop=4 shiftwidth=4 expandtab: */
