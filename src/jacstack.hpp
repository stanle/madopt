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
#ifndef MADOPT_JACSTACK
#define MADOPT_JACSTACK

#include <vector>
#include <string.h>

#include "liststack.hpp"
#include "common.hpp"

namespace MadOpt {
using namespace std;

class JacStack : public ListStack<Idx> {
    public:
        void emplace_back(const Idx& id, const double& value);

        void setXSize(const Idx& size);

        void mulAll(const double& value);

    private:
        vector<Idx> last_pos_map;
        vector<Idx> var_map;

        void clearLastStackPos();

        Idx getAndUpdateLastStackPos(const Idx& id, const Idx& new_conflict_pos);

        void setLastStackPos(const Idx& id, const Idx& conflict);
};


}
#endif
