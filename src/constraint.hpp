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
#ifndef MADOPT_CONSTRAINT_H
#define MADOPT_CONSTRAINT_H

#include "inner_constraint.hpp"

namespace MadOpt {

class Model;

//! constraint class to access/change constraint bounds and access dual/lambda
//solution values, can only be constructed via Model::addConstr()
class Constraint {
    public: 
        Constraint(){} // for python interface
        Constraint(Model* model, Idx pos): model(model), pos(pos){}

        //! get the lower bound
        double lb();

        //! set the lower bound
        void lb(double v);

        //! get the upper bound
        double ub();

        //! set the upper bound
        void ub(double v);

        //! get the lambda value of the current solution, only available
        double lam()const; 

    private:
        Model* model;
        Idx pos;
};

}
#endif
/* ex: set tabstop=4 shiftwidth=4 expandtab: */
