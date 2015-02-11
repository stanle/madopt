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
#ifndef MADOPT_VAR_H
#define MADOPT_VAR_H

#include "expr.hpp"
namespace MadOpt {

class InnerVar;

//! var class, should only be constructed via Model::addVar(); Model::addBVar() or Model::addIVar()
class Var: public Expr{
    public:
        Var();

        Var(InnerVar* var);

        Var(const Expr& e);

        Var(Expr&& e);

        //! get lower bound
        double lb();

        //! get upper bound
        double ub();

        //! set lower bound
        void lb(double v);

        //! set upper bound
        void ub(double v);

        //! get solution value
        double x()const ;

        //! get solution value if one is loaded, otherwise returns nan
        double v()const ;

        //! get init value
        double init();

        //! set init value
        void init(double v);

        Idx getPos()const ;

        //! get name
        string name()const;

        //! set solution value as init value
        void solAsInit();

        //! returns the status whether or not the variable is fixed
        bool fixed()const ;

        //! if true the variable is fixed to the init value
        void fixed(bool s);

    private:
        InnerVar* getIVar()const ;
};

}
#endif
/* ex: set tabstop=4 shiftwidth=4 expandtab: */
