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
#ifndef MADOPT_ARRAY
#define MADOPT_ARRAY

#include <vector>
#include "common.hpp"

namespace MadOpt {

template<class T>
class Array {
    public:
        Array(const Idx init_size=0): 
            data(init_size), data_end(0){}

        const Idx& size()const {
            return data_end;
        }

        T& back(const Idx& pos=0){
            return data[data_end-pos-1];
        }

        const T& back(const Idx& pos=0)const {
            return data[data_end-pos-1];
        }

        T& pop(){
            return data[--data_end];
        }

        const T& pop()const {
            return data[--data_end];
        }

        T& get(const Idx& pos){
            return data[pos];
        }

        const T& get(const Idx& pos)const {
            return data[pos];
        }

        T& getEndAndPush(){
            ensureElem();
            return data[data_end++];
        }

        T& getEndAndPushSave(){
            return data[data_end++];
        }

        void push(const T& new_t){
            ensureElem();
            data[data_end++] = new_t;
        }

        void pushSave(const T& new_t){
            data[data_end++] = new_t;
        }

    private:
        std::vector<T> data;
        Idx data_end;

        void ensureElem(){
            if (data_end == data.size())
                data.resize(data_end+1);
        }
};
}
#endif

