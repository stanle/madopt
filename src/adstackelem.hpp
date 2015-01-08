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
#ifndef MADOPT_ADSTACKELEM_H
#define MADOPT_ADSTACKELEM_H

//#include "common.hpp"

#include "mempool.hpp"
#include "node.hpp"
#include "adlist.hpp"

namespace MadOpt {

typedef MemPool<Idx> JacMemPool;
typedef ADList<Idx> JacList;

typedef MemPool<PII> HessMemPool;
typedef ADList<PII> HessList;

struct ADStackElem{
    ADStackElem(JacMemPool& pj, HessMemPool& ph);
    ADStackElem(const ADStackElem& other);
    ADStackElem(ADStackElem&& other) noexcept;

    ~ADStackElem();

    void clear();

    void mulAll(double w);

    void emplace(const double& r, const Idx& idx);

    void emplaceSQR(const double& r, const Idx& idx);

    string toString()const;

    double g;
    JacList jac;
    HessList hess;
};
}
#endif
