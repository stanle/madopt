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

#include "inner_var.hpp"
#include "exceptions.hpp"
#include "solution.hpp"
#include "common.hpp"
#include <cmath>

namespace MadOpt {

InnerVar::InnerVar(double _lb, 
        double _ub, 
        double xi, 
        VarType type, 
        string name, 
        const Solution& sol):
        pos(-1), 
        _ub(_ub), 
        _lb(_lb), 
        xi(xi), 
        _name(name), 
        type(type), 
        sol(sol){
            checkBounds();
        }

bool InnerVar::isActive(){ 
    return true; 
}

string InnerVar::name()const {
    return _name;
}

VarType InnerVar::getType()const {
    return type;
}

const Idx& InnerVar::getPos()const {
    return pos; 
}

void InnerVar::setPos(Idx epos){
    pos = epos; 
}

double InnerVar::lb()const {
    if (is_fixed) return xi;
    return _lb;
}

double InnerVar::ub()const {
    if (is_fixed) return xi;
    return _ub;
}

void InnerVar::lb(double l){
    _lb=l; checkBounds(); 
}

void InnerVar::ub(double u){
    _ub=u; checkBounds(); 
}

double InnerVar::init()const {
    return xi;
}

void InnerVar::init(double v){
    xi=v;
    checkBounds(); 
}

double InnerVar::x()const {
    return sol.x(pos); 
}

void InnerVar::solAsInit(){
    xi = x(); 
}

void InnerVar::fixed(bool s){
    is_fixed=s; 
}

bool InnerVar::fixed()const {
    return is_fixed; 
}

const Solution& InnerVar::getSolution()const {
    return sol; 
}

const string InnerVar::toString()const {
    string st;
    if (type != VarType::CONTINUOUS)
        st = ":I";

    string lbstr = to_string(_lb);
    if (_lb == -INF)
        lbstr = "-INF";

    string ubstr = to_string(_ub);
    if (_ub == INF)
        ubstr = "INF";

    return "" 
        + lbstr + "<" 
        + _name 
        + st
        + "=" + to_string(xi) 
        + "<" + ubstr;
}

inline void InnerVar::checkBounds(){
    if (_lb > _ub)
        throw MadOptError("lb larger than ub for variable " + _name);
    if (_lb > xi || _ub < xi)
        throw MadOptError("init not in bounds var " + _name);
}


}
