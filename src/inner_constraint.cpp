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
#include "vstack.hpp"

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

void InnerConstraint::init(HessPosMap& hess_pos_map, ADStack& stack){
    TRACE_START;
    stack.clear();
    ASSERT(stack.size() == 0);
    computeFinalStack(stack);
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
    jac_entries.resize(jaclist.size());
    Idx i = 0;
    for (auto node=jaclist.begin(); node!=jaclist.end(); node=node->next()){
        jac_entries[i++] = node->idx;
    }
    TRACE_END;
}

void InnerConstraint::init(HessPosMap& hess_pos_map, VStack& stack){
    TRACE_START;
    stack.clear();
    ASSERT_EQ(stack.size(), 0);
    computeFinalStack(stack);
    ASSERT_EQ(stack.size(), 1);

    hess_map.clear();
    auto& hesslist = stack.hess_stack;
    for (Idx i=1; i<=hesslist.length(); i++){
        auto& p = hesslist.getStackElemId(i);
        auto it = hess_pos_map.find(p);
        if (it == hess_pos_map.end())
            hess_pos_map[p] = hess_pos_map.size() - 1;
        hess_map.push_back(hess_pos_map[p]);
    }
    hess.resize(hess_map.size());

    jac.resize(stack.getNNZ_Jac());
    jac_entries.resize(stack.getNNZ_Jac());
    auto& jaclist = stack.jac_stack;
    for (Idx i=1; i<=jaclist.length(); i++){
        jac_entries[i-1] = jaclist.getStackElemId(i);
    }
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

void InnerConstraint::setEvals(ADStack& stack){
    TRACE_START;
    stack.clear();

    ASSERT(stack.size() == 0);
    computeFinalStack(stack);
    ASSERT(stack.size() == 1);
    
    g = stack.back().g;
    VALGRIND_CONDITIONAL_JUMP_TEST(g);

    Idx i = 0;
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

void InnerConstraint::setEvals(VStack& stack){
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

#define MADOPTCASE(a) case OP_##a: case##a(stack); break;
void InnerConstraint::computeFinalStack(ADStack& stack){
    TRACE_START;
    for (auto& op: operators){
        switch(op){
            MADOPTCASE(VAR_POINTER)
            MADOPTCASE(CONST)
            MADOPTCASE(ADD)
            MADOPTCASE(MUL)
            MADOPTCASE(POW)
            MADOPTCASE(SQR_VAR)
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

void InnerConstraint::caseADD(ADStack& stack){
    TRACE_START;
    auto size = getNextCounter(stack.data_i);
    Idx steps;

    while (size > 1) {
        steps = size / 2;
        for (Idx i=0; i<steps; i++){
            ADStackElem& top = stack.pop_back();
            ADStackElem& res = stack.back(steps-1);
            res.hess.mergeInto(top.hess);
            res.jac.mergeInto(top.jac);
            res.g += top.g;
        }
        size -= steps;
    }
    TRACE_END;
}

 void InnerConstraint::caseMUL(ADStack& stack){
    TRACE_START;
    auto size = getNextCounter(stack.data_i);
    Idx steps;
    while (size > 1) {
        steps = size / 2;
        for (Idx i=0; i<steps; i++){
            ADStackElem& top = stack.pop_back();
            ADStackElem& res = stack.back(steps-1);
            res.hess.mergeInto(top.hess, top.g, res.g);
            res.hess.mergeInto(res.jac, top.jac);
            res.hess.mergeInto(top.jac, res.jac);
            res.jac.mergeInto(top.jac, top.g, res.g);
            res.g *= top.g;
        }
        size -= steps;
    }
    TRACE_END;
}

void InnerConstraint::caseVAR_POINTER(ADStack& stack){
    auto pos = getNextPos(stack.data_i);
    stack.emplace_back(stack.x[pos], pos);
}

void InnerConstraint::caseSQR_VAR(ADStack& stack){
    auto pos = getNextPos(stack.data_i);
    stack.emplace_backSQR(stack.x[pos], pos);
}

void InnerConstraint::casePARAM_POINTER(ADStack& stack){
    stack.emplace_back(getNextParamValue(stack.data_i));
}

void InnerConstraint::caseCONST(ADStack& stack){
    stack.emplace_back(getNextValue(stack.data_i));
}

void InnerConstraint::casePOW(ADStack& stack){
    double value = getNextValue(stack.data_i);
    ADStackElem& top = stack.back();
    double pow_hess(1);
    if (value != 2)
        pow_hess = std::pow(top.g, value-2);
    double hess = pow_hess * value * (value-1);
    double jac = pow_hess * top.g * value;
    top.g = pow_hess * top.g * top.g;
    top.hess.mergeInto(top.jac, top.jac, jac, hess);
    top.jac.mulAll(jac);
}

void InnerConstraint::caseSIN(ADStack& stack){
    ADStackElem& top = stack.back();
    double v1 = std::cos(top.g);
    top.g = std::sin(top.g);
    top.hess.mergeInto(top.jac, top.jac, v1, -top.g);
    top.jac.mulAll(v1);
}

void InnerConstraint::caseCOS(ADStack& stack){
    ADStackElem& top = stack.back();
    double v1 = -std::sin(top.g);
    top.g = std::cos(top.g);
    top.hess.mergeInto(top.jac, top.jac, v1, -top.g);
    top.jac.mulAll(v1);
}

void InnerConstraint::caseTAN(ADStack& stack){
    ADStackElem& top = stack.back();
    double v1 = 1 + std::pow(top.g, 2);
    top.g = ::tan(top.g);
    top.hess.mergeInto(top.jac, top.jac, v1, -top.g);
    top.jac.mulAll(v1);
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

//set<Idx> InnerConstraint::getVarsSet() {
//    set<Idx> varset;
//    Idx data_i = 0;
//    for (auto op: operators){
//        if (op == OP_VAR_POINTER
//                || op == OP_SQR_VAR){
//            varset.insert(getNextPos(data_i));
//        } else if (op == OP_POW 
//                || op == OP_ADD 
//                || op == OP_MUL
//                || op == OP_PARAM_POINTER
//                || op == OP_CONST)
//            data_i++;
//    }
//    return varset;
//}

void InnerConstraint::computeFinalStack(VStack& stack){
    TRACE_START;
    for (auto& op: operators){
        switch(op){
            MADOPTCASE(VAR_POINTER)
            MADOPTCASE(CONST)
            MADOPTCASE(ADD)
            MADOPTCASE(MUL)
            MADOPTCASE(POW)
            MADOPTCASE(SQR_VAR)
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

void InnerConstraint::caseADD(VStack& stack){
    TRACE_START;
    auto size = getNextCounter(stack.data_i);
    stack.doADD(size);
    TRACE_END;
}

void InnerConstraint::caseMUL(VStack& stack){
   TRACE_START;
   auto size = getNextCounter(stack.data_i);
   for (Idx i=0; i<size-1; i++){
       stack.doMUL();
   }
   TRACE_END;
}

void InnerConstraint::caseVAR_POINTER(VStack& stack){
   TRACE_START;
    auto pos = getNextPos(stack.data_i);
    stack.emplace_back(pos, stack.x[pos]);
   TRACE_END;
}

void InnerConstraint::caseSQR_VAR(VStack& stack){
   TRACE_START;
    auto pos = getNextPos(stack.data_i);
    stack.emplace_backSQR(pos, stack.x[pos]);
   TRACE_END;
}

void InnerConstraint::casePARAM_POINTER(VStack& stack){
    TRACE_START;
    stack.emplace_back(getNextParamValue(stack.data_i));
    TRACE_END;
}

void InnerConstraint::caseCONST(VStack& stack){
   TRACE_START;
    stack.emplace_back(getNextValue(stack.data_i));
   TRACE_END;
}

void InnerConstraint::casePOW(VStack& stack){
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

void InnerConstraint::caseSIN(VStack& stack){
   TRACE_START;
    double& g = stack.lastG();
    double v1 = std::cos(g);
    g = std::sin(g);
    stack.doUnaryOp(v1, -g);
   TRACE_END;
}

void InnerConstraint::caseCOS(VStack& stack){
   TRACE_START;
    double& g = stack.lastG();
    double v1 = -std::sin(g);
    g = std::cos(g);
    stack.doUnaryOp(v1, -g);
   TRACE_END;
}

void InnerConstraint::caseTAN(VStack& stack){
   TRACE_START;
    double& g = stack.lastG();
    double v1 = 1 + std::pow(g, 2);
    g = ::tan(g);
    stack.doUnaryOp(v1, -g);
   TRACE_END;
}

}
