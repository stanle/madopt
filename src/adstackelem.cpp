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

#include "adstackelem.hpp"

namespace MadOpt {

ADStackElem::ADStackElem(JacMemPool& pj, HessMemPool& ph): g(0), jac(pj), hess(ph){}

void ADStackElem::clear(){
    jac.clear(); hess.clear(); 
}

void ADStackElem::mulAll(double w){
    g*=w, jac.mulAll(w); hess.mulAll(w); 
}

void ADStackElem::emplace(const double& r, const Idx& idx){
    g=r;
    jac.emplace_front(idx, 1);
}

void ADStackElem::emplaceSQR(const double& r, const Idx& idx){
    g=r*r;
    jac.emplace_front(idx, 2*r);
    hess.emplace_front(PII(idx,idx), 2);
}

string ADStackElem::toString()const{ 
    string res = "g=" + doubleToString(g) + "|";
    res += "Jac: " + jac.toString() + "|";
    res += "Hess: " + hess.toString();
    return res;
}

}
