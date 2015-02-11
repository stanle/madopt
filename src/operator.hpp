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
#ifndef MADOPT_OPERATOR_H
#define MADOPT_OPERATOR_H

#include "common.hpp"
#include "inner_var.hpp"
#include "inner_param.hpp"
#include "exceptions.hpp"

#define OP_VAR_POINTER 0
#define OP_CONST 1
#define OP_ADD 2
#define OP_MUL 3
#define OP_POW 4
#define OP_PARAM_POINTER 5
#define OP_SIN 6
#define OP_COS 7
#define OP_TAN 8

#define OP_VAR_IDX 20
#define OP_MUL_CONST 21
#define OP_ADD_CONST 22

namespace MadOpt {

typedef char OPType;

class Operator{
    public:

        Operator(OPType t, InnerVar* var):type(t){ 
            checkVarPointer();
            value = reinterpret_cast<uintptr_t>(var); 
        }

        Operator(OPType t, InnerParam* var):type(t){ 
            checkParam();
            value = reinterpret_cast<uintptr_t>(var); 
        }

        Operator(OPType t, double v): type(t){ 
            checkDouble();
            setValue(v); 
        }

        Operator(OPType t, Idx p): type(t), value(p){
            if (t != OP_ADD &&  t != OP_MUL && t != OP_VAR_IDX)
                throw MadOptError("wrong use of Expression type for Idx");
        }

        Operator(OPType t, int p): type(t), value(p){
            if (p < 0 || (t != OP_ADD &&  t != OP_MUL && t != OP_VAR_IDX))
                throw MadOptError("wrong use of Expression type for int");
        }

        Operator(OPType t):type(t){
            checkNone();
        }

        const uintptr_t& getData() const {
            return value;
        }

        const OPType& getType()const {
            return type; 
        }

        const Idx getIndex()const { 
            checkVarIdx();
            return value; 
        }

        const Idx getCounter()const { 
            checkCounter();
            return value; 
        }

        double getValue()const { 
            checkDouble();
            return reinterpret_cast<const double&>(value); 
        }

        InnerVar* getIVar()const { 
            checkVarPointer();
            return reinterpret_cast<InnerVar*>(value); 
        }

        InnerParam* getIParam()const { 
            checkParam();
            return reinterpret_cast<InnerParam*>(value); 
        }

        void setValue(double v){ 
            checkDouble();
            value = *reinterpret_cast<uintptr_t*>(&v); 
        }

        void addToCounter(Idx v){ 
            checkCounter();
            value += v; 
        }

        void modifyValue(double v, bool op){
            checkDouble();
            if (op) setValue(getValue() * v);
            else setValue(getValue() + v);
        } 

        string toString(){
            return std::to_string(type) + ": c=" + std::to_string(value) + " v=" + std::to_string(getValue());
        }

    private:
        OPType type;
        uintptr_t value;

        void checkNone()const {
            if (type != OP_SIN && type != OP_COS && type != OP_TAN)
                throw MadOptError("wrong use of Expression type for none");
        }

        void checkDouble()const {
            if (type != OP_CONST 
                    && type != OP_POW
                    && type != OP_ADD_CONST
                    && type != OP_MUL_CONST)
                throw MadOptError("wrong use of Expression type");
        }

        void checkCounter()const {
            if (type != OP_ADD && type != OP_MUL)
                throw MadOptError("wrong use of Expression type");
        }

        void checkParam()const {
            if (type != OP_PARAM_POINTER)
                throw MadOptError("wrong use of Expression type");
        }

        void checkVarPointer()const {
            if (type != OP_VAR_POINTER)
                throw MadOptError("wrong use of Expression type");
        }

        void checkVarIdx()const {
            if (type != OP_VAR_IDX)
                throw MadOptError("wrong use of Expression type");
        }
};
}
#endif
/* ex: set tabstop=4 shiftwidth=4 expandtab: */
