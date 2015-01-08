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
#include "expr.hpp"
#include <cmath>

namespace MadOpt {

Expr::Expr(double constant){
    ops.emplace_front(OP_CONST, constant);
}

Expr::Expr(int constant){
    ops.emplace_front(OP_CONST, (double)constant);
}

Expr::Expr(const Expr& a, const double& b){ 
    if (b == 0){
        ops.emplace_front(OP_CONST, 1.);
    } else if (b == 1){
        *this = a;
    } else if (a.getType() == OP_VAR_POINTER && b == 2){
        ops.emplace_front(OP_SQR_VAR, a.front().getIVar());
    } else if (a.getType() == OP_SQR_VAR){
        ops.emplace_front(OP_VAR_POINTER, a.front().getIVar()); 
        ops.emplace_front(OP_POW, b*2.); 
    } else {
        *this = a;
        if (getType() == OP_POW){
            front().modifyValue(b, true);
        } else {
            ops.emplace_front(OP_POW, b); 
        }
    }
}

Expr::Expr(const Expr& a, int op){ 
    if (op != OP_SIN && op != OP_COS && op != OP_TAN)
        throw MadOptError("wrong use of Expression type");
    *this = a; 
    ops.emplace_front(op); 
}

Expr::Expr(const Expr& a, OPType op){ 
    if (op != OP_SIN && op != OP_COS && op != OP_TAN)
        throw MadOptError("wrong use of Expression type");
    *this = a; 
    ops.emplace_front(op); 
}

Expr& Expr::operator+(){
    return *this;
}

Expr& Expr::operator+=(const double& a){
    *this += Expr(a); 
    return *this; 
}

Expr& Expr::operator*=(const double& a){ 
    *this *= Expr(a);
    return *this; 
}

Expr Expr::operator+(const Expr& b)const {
    Expr tmp(*this);
    tmp += b; return tmp; 
}

Expr Expr::operator*(const Expr& b)const {
    Expr tmp(*this);
    tmp *= b;
    return tmp;
}

Expr& Expr::operator+=(const Expr& a){ 
    return addOrMulOp(a.isZero(), isZero(), a, OP_ADD, true);
}

Expr& Expr::operator+=(const Expr&& a){
    return addOrMulOp(a.isZero(),  isZero(), a, OP_ADD, true);
}                                 

Expr& Expr::operator*=(const Expr& a){  
    if (isZero()) return *this;
    if (a.isZero()){ *this = a; return *this; }
    return addOrMulOp(a.isOne(), isOne(), a, OP_MUL, false);
}

Expr& Expr::operator*=(const Expr&& a){ 
    if (isZero()) return *this;
    if (a.isZero()){ *this = a; return *this; }
    return addOrMulOp(a.isOne(), isOne(), a, OP_MUL, false);
}

//
//
//
//
//
//
Expr operator+(const Expr& a, const double& b){
    Expr tmp(a);
    tmp += b;
    return tmp;
}

Expr operator*(const Expr& a, const double& b){
    Expr tmp(a);
    tmp *= b;
    return tmp;
}

Expr operator+(const double& a, const Expr& b){
    return Expr(a) + b;
}

Expr operator*(const double& a, const Expr& b){
    return Expr(a) * b;
}

Expr operator-(const Expr& a){
    return Expr(-1) * a;
}

Expr operator-(const Expr& a, const Expr& b){
    return a + -b;
}

Expr operator-(const Expr& a, const double& b){
    return a + -b;
}

Expr pow(const Expr& a, const double& b){
    return Expr(a, b);
}

Expr operator/(const Expr& a, const Expr& b){
    return a * pow(b, -1);
}

Expr sin(const Expr& a){
    return Expr(a, OP_SIN);
}

Expr cos(const Expr& a){
    return Expr(a, OP_COS);
}

Expr tan(const Expr& a){
    return Expr(a, OP_TAN);
}

Expr sqrt(const Expr& a){
    return pow(a, 0.5);
}

std::ostream &operator<<(std::ostream &os, const Expr &a){
	return os << a.toString();
}

string Expr::opsToString()const {
    string res;
    for (auto op: ops)
        res += op.toString() + "\n";
    return res;
}

Expr& Expr::mulEqual(const Expr& a) {
    if (isZero()) return *this;
    if (a.isZero()){ *this = a; return *this; }
    return addOrMulOp(a.isOne(), isOne(), a, OP_MUL, false);
}

Expr& Expr::plusEqual(const Expr& a) { 
    return addOrMulOp(a.isZero(), isZero(), a, OP_ADD, true); 
}

bool Expr::isZero()const { 
    if (isConstant())
        return front().getValue() == 0;
    return false; 
}

bool Expr::isOne()const { 
    if (isConstant())
        return front().getValue() == 1;
    return false; 
}

set<InnerVar*> Expr::getInnerVariables()const {
    set<InnerVar*> vars;
    for (auto op: ops)
        if (op.getType() == OP_VAR_POINTER || op.getType() == OP_SQR_VAR)
            vars.insert(op.getIVar());
    return vars;
}

string Expr::toString()const {
    auto iter = ops.begin();
    return toString(iter);
}

const OPType& Expr::getType()const {
    return front().getType(); 
}

bool Expr::isConstant()const {
    return front().getType() == OP_CONST; 
}

const Operator& Expr::front()const {
    return ops.front(); 
}

Operator& Expr::front(){
    return ops.front(); 
}

double Expr::getConstantValue()const { 
    auto iter = ops.begin();
    if (not isConstant())
        iter++;
    return iter->getValue();
}

const list<Operator>::const_iterator Expr::begin()const {
    return ops.begin(); 
}

const list<Operator>::const_iterator Expr::end()const {
    return ops.end(); 
}

list<Operator>::iterator Expr::begin(){
    return ops.begin(); 
} 

list<Operator>::iterator Expr::end(){
    return ops.end(); 
}

Idx Expr::size()const {
    return ops.size(); 
}

const list<Operator>& Expr::getOps()const {
    return ops; 
} 

//! evaluate the expression using the values from the solution
double Expr::x()const {
    auto iter = ops.begin();
    return x(iter); 
}

string Expr::toString(list<Operator>::const_iterator& iter)const {
    string res;
    const Operator& op = *iter;
    switch(iter->getType()){
        case OP_VAR_POINTER:
            return op.getIVar()->name();
        case OP_ADD:
            return getContent(iter, "+");
        case OP_MUL:
            return getContent(iter, "*");
        case OP_CONST:
            return doubleToString(op.getValue());
        case OP_ADD_CONST:
            return doubleToString(op.getValue()) + "+" + toString(++iter);
        case OP_MUL_CONST:
            return doubleToString(op.getValue()) + "*" + toStringEnclosed(++iter);
        case OP_POW:
          res = toStringEnclosed(++iter);
            return res + "^" + doubleToString(op.getValue());
        case OP_COS:
            return "cos(" + toString(++iter) + ")";
        case OP_SIN:
            return "sin(" + toString(++iter) + ")";
        case OP_TAN:
            return "tan(" + toString(++iter) + ")";
        case OP_SQR_VAR:
            return op.getIVar()->name() + "^2";
        case OP_PARAM_POINTER:
            return "[" + op.getIParam()->name() + "]";
            //return op.getIParam()->name() + "[" + doubleToString(op.getIParam()->value()) + "]";
        default:
          assert(false);
  }
    return "false";
}

string Expr::toStringEnclosed(list<Operator>::const_iterator& iter)const{
    const OPType& t = iter->getType();
    if (t == OP_VAR_POINTER || t == OP_PARAM_POINTER ||
            t == OP_CONST || t == OP_MUL || t == OP_SIN || t == OP_COS || t == OP_TAN)
        return toString(iter);
    return "(" + toString(iter) + ")";
}

string Expr::getContent(list<Operator>::const_iterator& iter,
        string delimeter)const {
    const Operator& op = *iter;
    string res = toStringEnclosed(++iter);
    for (size_t i=1; i<op.getCounter(); i++)
        res += delimeter + toStringEnclosed(++iter);
    return res;
}

 Expr& Expr::addOrMulOp(bool x, bool y, const Expr& a, OPType type, bool op){
    if (x) return *this;
    if (y){ *this = a; return *this; }
    int offset = inner(a, type, op);
    auto iter = a.begin();
    for (int i=0; i<offset; i++)
        iter++;
    ops.insert(ops.end(), iter, a.end());
    return *this;
}

 Expr& Expr::addOrMulOp(bool x, bool y, Expr&& a, OPType type, bool op){
    if (x) return *this;
    if (y){ *this = a; return *this; }
    int offset = inner(a, type, op);
    auto iter = make_move_iterator(a.begin());
    for (int i=0; i<offset; i++)
        iter++;
    ops.insert(ops.end(),
            iter,
            make_move_iterator(a.end()));
    return *this;
}

 int Expr::inner(const Expr& a, OPType& type, bool& op){
    if (getType() != type)
        ops.emplace_front(type, 1);

    int offset = 0;
    int new_elems = 1;

    if (a.getType() == type){
        offset = 1;
        new_elems = a.front().getCounter();
    }
    front().addToCounter(new_elems);
    return offset;
}

double Expr::x(list<Operator>::const_iterator& iter)const {
    double tmp = 0.;
    const Operator& op = *iter;
    switch(iter->getType()){
        case OP_VAR_POINTER:
            return op.getIVar()->x();
        case OP_ADD:
            tmp = x(++iter);
            for (size_t i=1; i<op.getCounter(); i++)
                tmp += x(++iter);
            return tmp;
        case OP_MUL:
            tmp = x(++iter);
            for (size_t i=1; i<op.getCounter(); i++)
                tmp *= x(++iter);
            return tmp;
        case OP_CONST:
            return op.getValue();
        case OP_ADD_CONST:
            return op.getValue() + x(++iter);
        case OP_MUL_CONST:
            return op.getValue()*x(++iter);
        case OP_POW:
            return std::pow(x(++iter), op.getValue());
        case OP_SQR_VAR:
            return std::pow(op.getIVar()->x(), 2);
        case OP_PARAM_POINTER:
            return op.getIParam()->value();
        case OP_COS:
            return std::cos(x(++iter));
        case OP_SIN:
            return std::sin(x(++iter));
        case OP_TAN:
            return std::tan(x(++iter));
        default:
          assert(false);
  }
  assert(false);
  return -1;
}

}
/* ex: set tabstop=4 shiftwidth=4 expandtab: */
