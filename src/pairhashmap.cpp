/* * Copyright 2014 National ICT Australia Limited (NICTA)
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

#include "pairhashmap.hpp"
#include "logger.hpp"

namespace MadOpt {
PairHashMap::PairHashMap(const Idx& max_range): max_range(max_range){
    mapping.resize(2*max_range);
}

Idx& PairHashMap::operator[](const HashPair& p){
    Idx key = p.first+ p.second;
    auto& bucket = mapping[key];

    if (bucket.size() == 1)
        return bucket.back().value;

    for (auto& ht: bucket){
        if (ht.p == p)
            return ht.value;
    }
    bucket.emplace_back(p, 0);
    return bucket.back().value;
}

void PairHashMap::resize(const Idx& max_range){
    this->max_range = max_range;
    mapping.resize(2*max_range);
}

string PairHashMap::str()const{
    string res;
    for (auto& bucket: mapping)
        for (auto& elem: bucket)
            res += to_string(elem.p) + ":" + to_string(elem.value) + " ";
    return res;
}

}
