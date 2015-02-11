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

#include "var.hpp"
#include "common.hpp"

namespace MadOpt {

Var::Var(){}

Var::Var(InnerVar* var){
    ops.emplace_front(OP_VAR_POINTER, var); 
}

Var::Var(const Expr& e): Expr(e){}

Var::Var(Expr&& e): Expr(e){}

//! get lower bound
double Var::lb(){
    return getIVar()->lb(); 
}

//! get upper bound
double Var::ub(){
    return getIVar()->ub(); 
}

//! set lower bound
void Var::lb(double v){
    getIVar()->lb(v); 
}

//! set upper bound
void Var::ub(double v){
    getIVar()->ub(v); 
}

//! get solution value
double Var::x()const {
    return getIVar()->x(); 
}

//! get solution value if one is loaded, otherwise returns nan
double Var::v()const {
    return getIVar()->v(); 
}

//! get init value
double Var::init(){
    return getIVar()->init(); 
}

//! set init value
void Var::init(double v){
    getIVar()->init(v); 
}

Idx Var::getPos()const {
    return getIVar()->getPos(); 
}

//! get name
string Var::name()const{
    return getIVar()->name(); 
}

//! set solution value as init value
void Var::solAsInit(){
    getIVar()->solAsInit(); 
}

//! returns the status whether or not the variable is fixed
bool Var::fixed()const {
    return getIVar()->fixed(); 
}

//! if true the variable is fixed to the init value
void Var::fixed(bool s){
    getIVar()->fixed(s); 
}

InnerVar* Var::getIVar()const {
    return ops.front().getIVar(); 
}

}
