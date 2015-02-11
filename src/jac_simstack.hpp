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

#ifndef MADOPT_JACSIMSTACK
#define MADOPT_JACSIMSTACK

#include "common.hpp"
#include "list_simstack.hpp"

namespace MadOpt {

class JacSimStack : public ListSimStack<Idx> {
    public:
        void emplace_back(const Idx& id){
            auto& elem = stack.getEndAndPush();
            elem.id = id;
            Idx& e = last_pos_map[id];
            elem.conflict = e;
            e = stack.size()-1;
            positions.push(e);
            if (stack.size() > _max_size)
                _max_size = stack.size();
            ASSERT_LE(positions.back(), stack.size());
        }

        void setXSize(const Idx& size){
            last_pos_map.resize(size, 0);
        }

    private:
        vector<Idx> last_pos_map;

        void clearLastStackPos(){
            for (Idx i=1; i<stack.size(); i++)
                last_pos_map[stack[i].id] = 0;
        }

        void setLastStackPos(const Idx& id, const Idx& conflict){
            last_pos_map[id] = conflict;
        }
};
}
#endif

