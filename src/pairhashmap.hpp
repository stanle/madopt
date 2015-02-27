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

#ifndef MADOPT_PAIRHASHMAP
#define MADOPT_PAIRHASHMAP

#include <vector>
#include <map>

#include "common.hpp"

namespace MadOpt {

using namespace std;

typedef pair<Idx, Idx> HashPair;
class PairHashMap {
    public:
        PairHashMap(const Idx& max_range=0);

        Idx& operator[](const HashPair&);
        void resize(const Idx& max_range);

        string str()const;

    private:
        struct HashTuple {
            HashTuple(const HashPair& p, const Idx& value): p(p), value(value){}
            HashPair p;
            Idx value;
        };

        vector< vector<HashTuple> > mapping;

        Idx max_range;
};
}
#endif
