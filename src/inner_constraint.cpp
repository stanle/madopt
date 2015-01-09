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

#include <stdlib.h>
#include <cmath>
#include "inner_constraint.hpp"
#include "logger.hpp"
#include "exceptions.hpp"
#include "solution.hpp"
#include "expr.hpp"
#include "common.hpp"
#include "adstack.hpp"
#include "adstackelem.hpp"
#include "operator.hpp"

namespace MadOpt {

void InnerConstraint::eval_h(double* values, const double& lambda){
    ASSERT_EQ(hess.size(), hess_map.size());
    for (Idx i=0; i<hess.size(); i++){
        VALGRIND_CONDITIONAL_JUMP_TEST(hess[i]);
        VALGRIND_CONDITIONAL_JUMP_TEST(hess_map[i]);
        values[hess_map[i]] += lambda * hess[i];
    }
}

const double& InnerConstraint::getG()const { 
    return g; 
}

const vector<double>& InnerConstraint::getJac()const { 
    return jac;
}

const vector<double>& InnerConstraint::getHess()const {
    return hess;
}

const vector<Idx>& InnerConstraint::getHessMap()const{
    return hess_map;
}


InnerConstraint::InnerConstraint(const Expr& expr, const double _lb, const double _ub, ADStack& stack): 
    _lb(_lb), 
    _ub(_ub),
    stack(stack)
{
    auto& ops = expr.getOps();
    for (auto iter=ops.rbegin(); iter!=ops.rend(); iter++){
        auto op = *iter;
        auto type = op.getType();
        operators.push_back(type);
        if (type == OP_VAR_POINTER
                || type == OP_SQR_VAR
                || type == OP_ADD
                || type == OP_MUL
                || type == OP_POW
                || type == OP_CONST
                || type == OP_VAR_IDX
                || type == OP_PARAM_POINTER){
            data.push_back(op.getData());
        } else {
            ASSERT(type == OP_COS 
                    || type == OP_SIN 
                    || type == OP_TAN, 
                    "unknown type", type);
        }
    }
}

InnerConstraint::InnerConstraint(const Expr& expr, ADStack& stack): 
    InnerConstraint(expr, 0, 0, stack){}

double InnerConstraint::lb(){
    return _lb; 
}

void InnerConstraint::lb(double v){
    _lb=v;
}

double InnerConstraint::ub(){
    return _ub; 
}

void InnerConstraint::ub(double v){
    _ub=v;
}

Idx InnerConstraint::getNNZ_Jac(){
    ASSERT(operators[0] == OP_CONST || !jac.empty());
    return jac.size(); 
}

void InnerConstraint::init(HessPosMap& hess_pos_map, const double* x){
    TRACE_START;
    stack.clear();
    ASSERT(stack.size() == 0);
    computeFinalStack(x);
    ASSERT(stack.size() == 1);

    hess_map.clear();
    auto& hesslist = stack.back().hess;
    for (auto node=hesslist.begin(); node!=hesslist.end(); node=node->next()){
        auto p = node->idx;
        auto it = hess_pos_map.find(p);
        if (it == hess_pos_map.end())
            hess_pos_map[p] = hess_pos_map.size() - 1;
        hess_map.push_back(hess_pos_map[p]);
    }
    hess.resize(hess_map.size());

    jac.clear();
    auto& jaclist = stack.back().jac;
    jac.resize(jaclist.size());
    TRACE_END;
}

vector<Idx> InnerConstraint::getJacEntries(){ 
    vector<Idx> jac_entries(getNNZ_Jac());
    getNZ_Jac(jac_entries.data());
    return jac_entries;
}

void InnerConstraint::getNZ_Jac(unsigned int* jCol){
    auto varset = getVarsSet();
    ASSERT_EQ(varset.size(), getNNZ_Jac());
    Idx data_i = 0;
    for (auto idx : varset) 
        jCol[data_i++] = idx;
}

void InnerConstraint::setEvals(const double* x){
    TRACE_START;
    ASSERT(x != nullptr);
    stack.clear();

    ASSERT(stack.size() == 0);
    computeFinalStack(x);
    ASSERT(stack.size() == 1);
    
    g = stack.back().g;
    VALGRIND_CONDITIONAL_JUMP_TEST(g);

    int i = 0;
    JacList& jaclist= stack.back().jac;
    for (auto node=jaclist.begin(); node!=jaclist.end(); node=node->next()){
        VALGRIND_CONDITIONAL_JUMP_TEST(node->value);
        jac[i++] = node->value;
    }

    i = 0;
    HessList& hesslist = stack.back().hess;
    for (auto node=hesslist.begin(); node!=hesslist.end(); node=node->next()){
        VALGRIND_CONDITIONAL_JUMP_TEST(node->value);
        hess[i++] = node->value;
    }
    TRACE_END;
}

string InnerConstraint::opsToString()const{
    string res;
    for (auto op: operators)
        res += to_string(op) + "\n";
    return res;
}

const string InnerConstraint::toString() const {
    return opsToString();
}

void InnerConstraint::computeFinalStack(const double* x){
    TRACE_START;
    ASSERT(x != nullptr);
    Idx data_i = 0;
    for (auto& op: operators){
        switch(op){
            case OP_VAR_POINTER:
                caseVAR(x, getNextPos(data_i));
                break;

            case OP_CONST:
                caseCONST(getNextValue(data_i));
                break;

            case OP_ADD:
                caseADD(getNextCounter(data_i));
                break;

            case OP_MUL:
                caseMUL(getNextCounter(data_i));
                break;

            case OP_POW:
                casePOW(getNextValue(data_i));
                break;

            case OP_COS:
                caseCOS();
                break;

            case OP_SIN:
                caseSIN();
                break;

            case OP_TAN:
                caseTAN();
                break;

            case OP_SQR_VAR:{
                TRACE("Squared Variable");
                auto pos = getNextPos(data_i);
                stack.emplace_backSQR(x[pos], pos);
                break;
            }

            case OP_PARAM_POINTER:
                caseCONST(getNextParamValue(data_i));
                break;

            case OP_ADD_CONST:
                stack.back().g += getNextValue(data_i);
                break;

            case OP_MUL_CONST:
                stack.back().mulAll(getNextValue(data_i));
                break;

            default:
                throw MadOptError("type not known");
        }
    }
    TRACE_END;
}

void InnerConstraint::caseVAR(const double* x, const Idx& pos){
    stack.emplace_back(x[pos], pos);
}

void InnerConstraint::caseCONST(const double& value){
    stack.emplace_back(value);
}

 void InnerConstraint::caseADD(const Idx& counter){
    TRACE_START;
    TRACE("counter=", counter,
            "stack=\n", stack.toString());
    Idx stepsize = 1;
    auto iter = counter - 1;

    TRACE("start while loop");
    while (2*stepsize <= counter){
        iter = counter - 1;
        ldiv_t divres = ldiv(counter, 2*stepsize);

        TRACE("before for loop::",
                "iter=", iter,
                "stepsize=", stepsize, 
                "div quot=", divres.quot, 
                "div rest=", divres.rem,
                "stack=\n", stack.toString());

        for (int i=0; i <divres.quot ; ++i) {
            auto& m1 = stack.back(iter);
            iter -= stepsize;
            auto& iter_elem = stack.back(iter);
            m1.hess.mergeInto(iter_elem.hess);
            m1.jac.mergeInto(iter_elem.jac);
            m1.g += iter_elem.g;
            iter -= stepsize;
            TRACE("after i=", i, 
                    "iter=", iter, 
                    "Stack=\n", stack.toString());
        }

        TRACE("after for loop",
                "iter=", iter, 
                "Stack=\n", stack.toString());

        if (divres.rem >= stepsize){
            ASSERT_LE(iter, counter-1);
            auto& m1 = stack.back(iter + 2*stepsize);
            auto& iter_elem = stack.back(iter);
            TRACE("merge=", iter_elem.toString());
            TRACE("merge into=", m1.toString());
            m1.hess.mergeInto(iter_elem.hess);
            m1.jac.mergeInto( iter_elem.jac);
            m1.g += iter_elem.g;
        }

        stepsize *= 2;

        TRACE("after rest",
                "stepsize=", stepsize, 
                "Stack=\n", stack.toString());
    }

    TRACE("end while loop",
            "counter=", counter);
    stack.pop_back(counter-1);
    TRACE("End Stack=\n", stack.toString());
    TRACE_END;
}

 void InnerConstraint::caseMUL(const Idx& counter){
    TRACE_START;
    ADStackElem& res = stack.back(counter-1);
    for (Idx i=0; i<counter-1; i++){
        ADStackElem& top = stack.back();
        res.hess.mergeInto(top.hess, top.g, res.g);
        res.hess.mergeInto(res.jac, top.jac);
        res.hess.mergeInto(top.jac, res.jac);
        res.jac.mergeInto(top.jac, top.g, res.g);
        res.g *= top.g;
        stack.pop_back();
    }
    TRACE_END;
}

void InnerConstraint::caseSIN(){
    ADStackElem& top = stack.back();
    double _cos = ::cos(top.g);
    top.g = ::sin(top.g);
    doHessJacs(top, _cos, -top.g);
}

void InnerConstraint::caseCOS(){
    ADStackElem& top = stack.back();
    double _sin = ::sin(top.g);
    top.g = ::cos(top.g);
    doHessJacs(top, -_sin, -top.g);
}

void InnerConstraint::caseTAN(){
    ADStackElem& top = stack.back();
    top.g = ::tan(top.g);
    double _sec = 1 + ::pow(top.g, 2);
    doHessJacs(top, _sec, -top.g);
}

void InnerConstraint::casePOW(const double& value){
    ADStackElem& top = stack.back();
    double pow_hess(1);
    if (value != 2)
        pow_hess = std::pow(top.g, value-2);
    double hess = pow_hess * value * (value-1);
    double jac = pow_hess * top.g * value;
    top.g = pow_hess * top.g * top.g;
    doHessJacs(top, jac, hess);
}

void InnerConstraint::doHessJacs(ADStackElem& top, double frst, double scd){
    top.hess.mergeInto(top.jac, top.jac, frst, scd);
    top.jac.mulAll(frst);
}

double InnerConstraint::getNextValue(Idx& idx){
    return reinterpret_cast<const double&>(data[idx++]);
}

Idx InnerConstraint::getNextCounter(Idx& idx){
    return data[idx++];
}

Idx InnerConstraint::getNextPos(Idx& idx){
    return (reinterpret_cast<InnerVar*>(data[idx++]))->getPos();
}

double InnerConstraint::getNextParamValue(Idx& idx){
    return (reinterpret_cast<InnerParam*>(data[idx++]))->value();
}

set<Idx> InnerConstraint::getVarsSet() {
    set<Idx> varset;
    Idx data_i = 0;
    for (auto op: operators){
        if (op == OP_VAR_POINTER
                || op == OP_SQR_VAR){
            varset.insert(getNextPos(data_i));
        } else if (op == OP_POW 
                || op == OP_ADD 
                || op == OP_MUL
                || op == OP_PARAM_POINTER
                || op == OP_CONST)
            data_i++;
    }
    return varset;
}


}
