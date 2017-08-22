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
#ifndef MADOPT_VALUE_H
#define MADOPT_VALUE_H

#include "inner_var.hpp"
#include "inner_param.hpp"

namespace MadOpt {
    union Value
    {
        InnerVar* iVar;
        InnerParam* iParam;
        Idx idx;
        double d;
        int i;

        Value() : i(0) {}
        Value(InnerVar* x) : iVar(x) {}
        Value(InnerParam* x) : iParam(x) {}
        Value(Idx x) : idx(x) {}
        Value(double x) : d(x) {}
        Value(int x) : i(x) {}
    };
}

#endif
/* ex: set tabstop=4 shiftwidth=4 expandtab: */
