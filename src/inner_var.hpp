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
#ifndef MADOPT_INNER_VAR_H
#define MADOPT_INNER_VAR_H

#include "common.hpp"

namespace MadOpt {

class Solution;

class InnerVar{
    public:
        InnerVar(double _lb, 
                double _ub, 
                double xi,
                VarType type,
                string name,
                const Solution& sol);

        bool isActive();
        string name()const ;
        VarType getType()const ;
        const Idx& getPos()const ;
        void setPos(Idx epos);

        double lb()const ;

        double ub()const ;

        void lb(double l);
        void ub(double u);

        double init()const ;
        void init(double v);

        double x()const;
        double v()const;

        void solAsInit();

        void fixed(bool s);
        bool fixed()const ;

        const Solution& getSolution()const ;

        const string toString()const ;

    private:
        Idx pos;
        double _ub;
        double _lb;
        double xi;
        const string _name;
        VarType type;
        const Solution& sol;

        bool is_fixed=false;

        void checkBounds();
};
}
#endif
/* ex: set tabstop=4 shiftwidth=4 expandtab: */
