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
#include "logger.hpp"
#include <string.h>

namespace MadOpt {

template<class T>
class Array {
    public:
        Array(const Idx init_size=0): 
            data(init_size), data_end(init_size){}

        const Idx& size()const {
            return data_end;
        }

        T& back(const Idx& pos=1){
            ASSERT_LE(0, data_end-pos);
            return data[data_end-pos];
        }

        const T& back(const Idx& pos=1)const {
            ASSERT_LE(0, data_end-pos);
            return data[data_end-pos];
        }

        T& pop(const Idx& nof){
            ASSERT_LE(nof, data_end);
            data_end -= nof;
            return data[data_end];
        }

        T& pop(){
            ASSERT_LE(1, data_end);
            return data[--data_end];
        }

        T& get(const Idx& pos){
            ASSERT_BETWEEN(0, pos, data.size()-1);
            return data[pos];
        }

        const T& get(const Idx& pos)const {
            ASSERT_BETWEEN(0, pos, data.size()-1);
            return data[pos];
        }

        T& operator[](const Idx& pos){
            ASSERT_BETWEEN(0, pos, data.size()-1);
            return data[pos];
        }

        const T& operator[](const Idx& pos)const {
            ASSERT_BETWEEN(0, pos, data.size()-1);
            return data[pos];
        }

        Idx getEndPosAndPush(){
            ensureElem();
            return data_end++;
        }

        T& getEndAndPush(){
            ensureElem();
            return data[data_end++];
        }

        T& getEndAndPushSave(){
            ASSERT_LE(data_end, data.size()-1);
            return data[data_end++];
        }

        void push(const T& new_t){
            ensureElem();
            data[data_end++] = new_t;
        }

        void pushSave(const T& new_t){
            ASSERT_LE(data_end, data.size()-1);
            data[data_end++] = new_t;
        }

        void clear(){
            data_end = 0;
            iter=0;
        }

        void reset(){
            iter=0;
        }

        T& next(){
            return data[iter++];
        }

        T& current(){
            return data[iter];
        }

        const Idx& current_pos()const {
            return iter;
        }

        void resize(const Idx& new_size){
            data.resize(new_size);
        }

        std::string str()const {
            std::string res;
            for (Idx i=0; i<data_end; i++){
                res += to_string(data[i]);
                if (i == iter)
                    res += "*";
                res += " ";
            }
            return res;
        }

    private:
        std::vector<T> data;
        Idx data_end=0;
        Idx iter = 0;

        void ensureElem(){
            if (data_end == data.size())
                data.resize(data_end+1);
        }
};
}
#endif

