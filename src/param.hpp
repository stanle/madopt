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
#ifndef MADOPT_PARAM_H
#define MADOPT_PARAM_H

#include "expr.hpp"

namespace MadOpt {

class InnerParam;

//! parameter class, can only be constructed via Model::addParam()
class Param: public Expr{
    public:
        Param(){} // for python interface

        Param(InnerParam* var): Expr(true, true){
            ops.emplace_front(OP_PARAM_POINTER, var);
        }

        Param(const Expr& e): Expr(e){}

        Param(Expr&& e): Expr(e){}

        //! get parameter name
        string name()const{
            return getIParam()->name(); 
        }

        //! set value
        void value(const double v){
            getIParam()->value(v); 
        }

        //! get value
        double value() const {
            return getIParam()->value(); 
        }

    private:
        InnerParam* getIParam()const {
            return ops.front().getIParam(); 
        }
};
}
#endif
/* ex: set tabstop=4 shiftwidth=4 expandtab: */

