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

#include "econstraint.hpp"
#include "expr.hpp"
#include "common.hpp"
#include "adstack.hpp"
#include "adstackelem.hpp"
#include "logger.hpp"
#include "exceptions.hpp"

namespace MadOpt {

EConstraint::EConstraint(const Expr& expr, const double _lb, const double _ub, ADStack& stack): 
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

EConstraint::EConstraint(const Expr& expr, ADStack& stack): 
    EConstraint(expr, 0, 0, stack){}

double EConstraint::lb(){
    return _lb; 
}

void EConstraint::lb(double v){
    _lb=v;
}

double EConstraint::ub(){
    return _ub; 
}

void EConstraint::ub(double v){
    _ub=v;
}

Idx EConstraint::getNNZ_Jac(){
    ASSERT(operators[0] == OP_CONST || !jac.empty());
    return jac.size(); 
}

void EConstraint::init(HessPosMap& hess_pos_map){
    stack.clear();
    ASSERT(stack.size() == 0);
    computeFinalStack();
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
}

vector<Idx> EConstraint::getJacEntries(){ 
    vector<Idx> jac_entries(getNNZ_Jac());
    getNZ_Jac(jac_entries.data());
    return jac_entries;
}

void EConstraint::getNZ_Jac(unsigned int* jCol){
    auto varset = getVarsSet();
    ASSERT_EQ(varset.size(), getNNZ_Jac());
    Idx data_i = 0;
    for (auto idx : varset) 
        jCol[data_i++] = idx;
}

void EConstraint::setEvals(const double* x){
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

string EConstraint::opsToString()const{
    string res;
    for (auto op: operators)
        res += to_string(op) + "\n";
    return res;
}

const string EConstraint::toString() const {
    return opsToString();
}

inline double EConstraint::getX(const double* x, Idx index)const{
    return (x==nullptr) ? (0) : (x[index]);
}

void EConstraint::computeFinalStack(const double* x){
    Idx data_i = 0;
    for (auto& op: operators){
        switch(op){
            case OP_VAR_POINTER:{
                auto pos = getNextPos(data_i);
                stack.emplace_back(getX(x, pos), pos);
                break;
            }

            case OP_SQR_VAR:{
                auto pos = getNextPos(data_i);
                stack.emplace_backSQR(getX(x, pos), pos);
                break;
            }

            case OP_CONST:
                stack.emplace_back(getNextValue(data_i));
                break;

            case OP_PARAM_POINTER:
                stack.emplace_back(getNextParamValue(data_i));
                break;

            case OP_ADD_CONST:
                stack.back().g += getNextValue(data_i);
                break;

            case OP_MUL_CONST:
                stack.back().mulAll(getNextValue(data_i));
                break;

            case OP_ADD:
                caseADD(getNextCounter(data_i));
                break;

            case OP_MUL:
                caseMUL(getNextCounter(data_i));
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

            case OP_POW:
                casePOW(getNextValue(data_i));
                break;

            default:
                throw MadOptError("type not known");
        }
    }
}

inline void EConstraint::caseADD(const Idx& counter){
    TRACE_START;
    TRACE("counter=", counter);
    Idx stepsize = 1;
    auto frst_pos = stack.backIterator(counter-1);
    auto iter = frst_pos;

    while (2*stepsize <= counter){
        iter = frst_pos;
        ldiv_t divres = ldiv(counter, 2*stepsize);
        TRACE("Stack=\n", stack.toString());
        TRACE("stepsize=", stepsize, 
                "div quot=", divres.quot, 
                "div rest=", divres.rem);
        for (int i=0; i <divres.quot ; ++i) {
            auto m1 = iter;
            iter += stepsize;
            m1->hess.mergeInto(iter->hess);
            m1->jac.mergeInto(iter->jac);
            m1->g += iter->g;
            iter += stepsize;
            TRACE("after i=", i, "Stack=\n", stack.toString());
        }

        TRACE("after loop Stack=\n", stack.toString());
        if (divres.rem >= stepsize){
            auto m1 = iter - 2*stepsize;
            TRACE("merge=", iter->toString());
            TRACE("merge into=", m1->toString());
            m1->hess.mergeInto(iter->hess);
            m1->jac.mergeInto(iter->jac);
            m1->g += iter->g;
        }
        TRACE("after rest Stack=\n", stack.toString());

        stepsize *= 2;
    }
    stack.pop_back(counter-1);
    TRACE("End Stack=\n", stack.toString());
    TRACE_END;
}

inline void EConstraint::caseMUL(const Idx& counter){
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
}

inline void EConstraint::caseSIN(){
    ADStackElem& top = stack.back();
    double _cos = ::cos(top.g);
    top.g = ::sin(top.g);
    doHessJacs(top, _cos, -top.g);
}

inline void EConstraint::caseCOS(){
    ADStackElem& top = stack.back();
    double _sin = ::sin(top.g);
    top.g = ::cos(top.g);
    doHessJacs(top, -_sin, -top.g);
}

inline void EConstraint::caseTAN(){
    ADStackElem& top = stack.back();
    top.g = ::tan(top.g);
    double _sec = 1 + ::pow(top.g, 2);
    doHessJacs(top, _sec, -top.g);
}

inline void EConstraint::casePOW(const double& value){
    ADStackElem& top = stack.back();
    double pow_hess(1);
    if (value != 2)
        pow_hess = std::pow(top.g, value-2);
    double hess = pow_hess * value * (value-1);
    double jac = pow_hess * top.g * value;
    top.g = pow_hess * top.g * top.g;
    doHessJacs(top, jac, hess);
}

inline void EConstraint::doHessJacs(ADStackElem& top, double frst, double scd){
    top.hess.mergeInto(top.jac, top.jac, frst, scd);
    top.jac.mulAll(frst);
}

inline double EConstraint::getNextValue(Idx& idx){
    return reinterpret_cast<const double&>(data[idx++]);
}

inline Idx EConstraint::getNextCounter(Idx& idx){
    return data[idx++];
}

inline Idx EConstraint::getNextPos(Idx& idx){
    return (reinterpret_cast<InnerVar*>(data[idx++]))->getPos();
}

inline double EConstraint::getNextParamValue(Idx& idx){
    return (reinterpret_cast<InnerParam*>(data[idx++]))->value();
}

set<Idx> EConstraint::getVarsSet() {
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
