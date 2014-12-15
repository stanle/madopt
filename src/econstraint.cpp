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

namespace MadOpt {

EConstraint::EConstraint(const Expr& expr, const double _lb, const double _ub): 
    _lb(_lb), 
    _ub(_ub), 
    ops(expr.getOps().rbegin(), expr.getOps().rend())
{}

EConstraint::EConstraint(const Expr& expr): 
    EConstraint(expr, 0, 0){}

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

void EConstraint::setStack(ADStack* stack){
    this->stack = stack; 
}

Idx EConstraint::getNNZ_Jac(){
    return jac.size(); 
}

void EConstraint::init(HessPosMap& hess_pos_map){
    assert(stack != nullptr);
    stack->clear();
    assert(stack->size() == 0);
    computeFinalStack();
    assert(stack->size() == 1);

    hess_map.clear();
    auto& hesslist = stack->back().hess;
    for (auto node=hesslist.begin(); node!=hesslist.end(); node=node->next()){
        auto p = node->idx;
        auto it = hess_pos_map.find(p);
        if (it == hess_pos_map.end())
            hess_pos_map[p] = hess_pos_map.size() - 1;
        hess_map.push_back(hess_pos_map[p]);
    }
    hess.resize(hess_map.size());

    jac.clear();
    auto& jaclist = stack->back().jac;
    //jac.resize(jaclist.size());
    for (auto node=jaclist.begin(); node!=jaclist.end(); node=node->next())
        jac.push_back(reinterpret_cast<const double&>(node->idx));
}

vector<Idx> EConstraint::getJacEntries(){ 
    vector<Idx> jac_entries(jac.size());
    for (Idx i=0; i<jac.size(); i++){
        jac_entries[i] = readJacEntry(i);
    }
    return jac_entries;
}

void EConstraint::getNZ_Jac(int* jCol){
    set<Idx> varset;
    for (auto op: ops)
        if (op.getType() == OP_VAR_POINTER)
            varset.insert(op.getIVar()->getPos());

    int i=0;
    for (auto idx : varset) 
        jCol[i++] = idx;
}

void EConstraint::setEvals(const double* x){
    TRACE_START;
    assert(x != nullptr);
    assert(stack != nullptr);
    stack->clear();

    assert(stack->size() == 0);
    computeFinalStack(x);
    assert(stack->size() == 1);
    
    g = stack->back().g;
    VALGRIND_CONDITIONAL_JUMP_TEST(g);

    int i = 0;
    JacList& jaclist= stack->back().jac;
    for (auto node=jaclist.begin(); node!=jaclist.end(); node=node->next()){
        VALGRIND_CONDITIONAL_JUMP_TEST(node->value);
        jac[i++] = node->value;
    }

    i = 0;
    HessList& hesslist = stack->back().hess;
    for (auto node=hesslist.begin(); node!=hesslist.end(); node=node->next()){
        VALGRIND_CONDITIONAL_JUMP_TEST(node->value);
        hess[i++] = node->value;
    }
    TRACE_END;
}

string EConstraint::opsToString()const{
    string res;
    for (auto op: ops)
        res += op.toString() + "\n";
    return res;
}

const string EConstraint::toString() const {
    return opsToString();
}

inline double EConstraint::getX(const double* x, Idx index)const{
    return (x==nullptr) ? (0) : (x[index]);
}

void EConstraint::computeFinalStack(const double* x){
    for (auto& op: ops){
        switch(op.getType()){
            case OP_VAR_POINTER:
                stack->emplace_back(getX(x, op.getIVar()->getPos()),
                        op.getIVar()->getPos());
                break;

            case OP_VAR_IDX:
                stack->emplace_back(getX(x, op.getIndex()),
                        op.getIndex());
                break;

            case OP_SQR_VAR:
                stack->emplace_backSQR(getX(x, op.getIVar()->getPos()),
                        op.getIVar()->getPos());
                break;

            case OP_CONST:
                stack->emplace_back(op.getValue());
                break;

            case OP_PARAM_POINTER:
                stack->emplace_back(op.getIParam()->value());
                break;

            case OP_ADD_CONST:
                stack->back().g += op.getValue();
                break;

            case OP_MUL_CONST:
                stack->back().mulAll(op.getValue());
                break;

            case OP_ADD:
                caseADD(op);
                break;

            case OP_MUL:
                caseMUL(op);
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
                casePOW(op);
                break;
        }
    }
}

inline void EConstraint::caseADD(const Operator& op){
    TRACE_START;
    Idx counter = op.getCounter();
    TRACE("counter=", counter);
    Idx stepsize = 1;
    auto frst_pos = stack->backIterator(counter-1);
    auto iter = frst_pos;

    while (2*stepsize <= counter){
        iter = frst_pos;
        ldiv_t divres = ldiv(counter, 2*stepsize);
        TRACE("Stack=\n", stack->toString());
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
            TRACE("after i=", i, "Stack=\n", stack->toString());
        }

        TRACE("after loop Stack=\n", stack->toString());
        if (divres.rem >= stepsize){
            auto m1 = iter - 2*stepsize;
            TRACE("merge=", iter->toString());
            TRACE("merge into=", m1->toString());
            m1->hess.mergeInto(iter->hess);
            m1->jac.mergeInto(iter->jac);
            m1->g += iter->g;
        }
        TRACE("after rest Stack=\n", stack->toString());

        stepsize *= 2;
    }
    stack->pop_back(counter-1);
    TRACE("End Stack=\n", stack->toString());
    TRACE_END;
}

inline void EConstraint::caseMUL(const Operator& op){
    ADStackElem& res = stack->back(op.getCounter()-1);
    for (Idx i=0; i<op.getCounter()-1; i++){
        ADStackElem& top = stack->back();
        res.hess.mergeInto(top.hess, top.g, res.g);
        res.hess.mergeInto(res.jac, top.jac);
        res.hess.mergeInto(top.jac, res.jac);
        res.jac.mergeInto(top.jac, top.g, res.g);
        res.g *= top.g;
        stack->pop_back();
    }
}

inline void EConstraint::caseSIN(){
    ADStackElem& top = stack->back();
    double _cos = ::cos(top.g);
    top.g = ::sin(top.g);
    doHessJacs(top, _cos, -top.g);
}

inline void EConstraint::caseCOS(){
    ADStackElem& top = stack->back();
    double _sin = ::sin(top.g);
    top.g = ::cos(top.g);
    doHessJacs(top, -_sin, -top.g);
}

inline void EConstraint::caseTAN(){
    ADStackElem& top = stack->back();
    top.g = ::tan(top.g);
    double _sec = 1 + ::pow(top.g, 2);
    doHessJacs(top, _sec, -top.g);
}

inline void EConstraint::casePOW(const Operator& op){
    ADStackElem& top = stack->back();
    double pow_hess(1);
    if (op.getValue() != 2)
        pow_hess = std::pow(top.g, op.getValue()-2);
    double hess = pow_hess * op.getValue() * (op.getValue()-1);
    double jac = pow_hess * top.g * op.getValue();
    top.g = pow_hess * top.g * top.g;
    doHessJacs(top, jac, hess);
}

inline void EConstraint::doHessJacs(ADStackElem& top, double frst, double scd){
    top.hess.mergeInto(top.jac, top.jac, frst, scd);
    top.jac.mulAll(frst);
}

inline Idx EConstraint::readJacEntry(Idx index){
     return reinterpret_cast<const Idx&>(jac[index]);
}

}
