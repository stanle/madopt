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
#include "operator.hpp"
#include "stack.hpp"
#include "simstack.hpp"
#include "cstack.hpp"

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

InnerConstraint::InnerConstraint(const Expr& expr, const double _lb, const double _ub): 
    _lb(_lb), 
    _ub(_ub)
{
    auto& ops = expr.getOps();
    for (auto iter=ops.rbegin(); iter!=ops.rend(); iter++){
        auto op = *iter;
        auto type = op.getType();

        operators.push_back(type);
        if (type == OP_VAR_POINTER
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

InnerConstraint::InnerConstraint(const Expr& expr): 
    InnerConstraint(expr, 0, 0){}

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

void InnerConstraint::init(HessPosMap& hess_pos_map, SimStack& stack){
    TRACE_START;
    stack.clear();
    ASSERT_EQ(stack.size(), 0);
    computeFinalStack(stack);
    ASSERT_EQ(stack.size(), 1);

    hess_map.clear();
    vector<PII> hess_entries = stack.getHessEntries();
    for (auto& p : hess_entries){
        auto it = hess_pos_map.find(p);
        if (it == hess_pos_map.end())
            hess_pos_map[p] = hess_pos_map.size() - 1;
        hess_map.push_back(hess_pos_map[p]);
    }
    hess.resize(hess_map.size());

    jac_entries = stack.getJacEntries();
    jac.resize(jac_entries.size());
    TRACE_END;
}

const vector<Idx>& InnerConstraint::getJacEntries(){ 
    return jac_entries;
}

void InnerConstraint::getNZ_Jac(unsigned int* jCol){
    Idx data_i = 0;
    for (auto id : jac_entries)
        jCol[data_i++] = id;
}

void InnerConstraint::setEvals(CStack& stack){
    TRACE_START;
    stack.clear();

    ASSERT(stack.size() == 0);
    computeFinalStack(stack);
    ASSERT(stack.size() == 1);

    g = stack.lastG();
    VALGRIND_CONDITIONAL_JUMP_TEST(g);

    stack.fill(jac.data(), hess.data());
    TRACE_END;
}

const double& InnerConstraint::getNextValue(Idx& idx){
    return reinterpret_cast<const double&>(data[idx++]);
}

Idx InnerConstraint::getNextCounter(Idx& idx){
    return data[idx++];
}

const Idx& InnerConstraint::getNextPos(Idx& idx){
    return (reinterpret_cast<InnerVar*>(data[idx++]))->getPos();
}

const double& InnerConstraint::getNextParamValue(Idx& idx){
    return (reinterpret_cast<InnerParam*>(data[idx++]))->value();
}

#define MADOPTCASE(a) case OP_##a: case##a(stack); break;
void InnerConstraint::computeFinalStack(Stack& stack){
    TRACE_START;
    for (auto& op: operators){
        switch(op){
            MADOPTCASE(VAR_POINTER)
            MADOPTCASE(CONST)
            MADOPTCASE(ADD)
            MADOPTCASE(MUL)
            MADOPTCASE(POW)
            MADOPTCASE(PARAM_POINTER)
            MADOPTCASE(SIN)
            MADOPTCASE(COS)
            MADOPTCASE(TAN)

            default:
                throw MadOptError("unknown operator type found");
        }
    }
    TRACE_END;
}

void InnerConstraint::caseADD(Stack& stack){
    TRACE_START;
    const auto& size = getNextCounter(stack.data_i);
    stack.doAdd(size);
    TRACE_END;
}

void InnerConstraint::caseMUL(Stack& stack){
   TRACE_START;
   const auto& size = getNextCounter(stack.data_i);
   for (Idx i=0; i<size-1; i++){
       stack.doMull();
   }
   TRACE_END;
}

void InnerConstraint::caseVAR_POINTER(Stack& stack){
   TRACE_START;
    const auto& pos = getNextPos(stack.data_i);
    stack.emplace_back(pos);
   TRACE_END;
}

void InnerConstraint::casePARAM_POINTER(Stack& stack){
    TRACE_START;
    stack.emplace_back(getNextParamValue(stack.data_i));
    TRACE_END;
}

void InnerConstraint::caseCONST(Stack& stack){
   TRACE_START;
    stack.emplace_back(getNextValue(stack.data_i));
   TRACE_END;
}

void InnerConstraint::casePOW(Stack& stack){
   TRACE_START;
    double value = getNextValue(stack.data_i);
    double& g = stack.lastG();
    double pow_hess(1);
    if (value != 2)
        pow_hess = std::pow(g, value-2);
    double hess = pow_hess * value * (value-1);
    double jac = pow_hess * g * value;
    g = pow_hess * g * g;
    stack.doUnaryOp(jac, hess);
   TRACE_END;
}

void InnerConstraint::caseSIN(Stack& stack){
   TRACE_START;
    double& g = stack.lastG();
    double v1 = std::cos(g);
    g = std::sin(g);
    stack.doUnaryOp(v1, -g);
   TRACE_END;
}

void InnerConstraint::caseCOS(Stack& stack){
   TRACE_START;
    double& g = stack.lastG();
    double v1 = -std::sin(g);
    g = std::cos(g);
    stack.doUnaryOp(v1, -g);
   TRACE_END;
}

void InnerConstraint::caseTAN(Stack& stack){
   TRACE_START;
    double& g = stack.lastG();
    double v1 = 1 + std::pow(g, 2);
    g = ::tan(g);
    stack.doUnaryOp(v1, -g);
   TRACE_END;
}

}
