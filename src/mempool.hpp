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

#ifndef MADOPT_MEMPOOL_H
#define MADOPT_MEMPOOL_H

#include "common.hpp"
#include "node.hpp"
#include "logger.hpp"

namespace MadOpt {

template<class T>
class MemPool{
    public:
        MemPool(Idx initsize=0): node_counter(initsize), balance_counter(0){
            pNode<T> tmp;
            for (Idx i=0; i<initsize; i++){
                tmp = new Node<T>();
                tmp->setNext(begin);
                begin = tmp;
            }
        } 

        ~MemPool(){
            if (not fixed)
                clear();
            else {
                ASSERT_EQ(node_counter, items.size());
            }
        }

        void setUnused(pNode<T> node){ 
            TRACE(node);
            node->setNext(begin);
            begin = node;
            balance_counter--;
            node_counter++;
        }

        pNode<T> getNewNode(pNode<T> next, T& idx, double& value){ 
            balance_counter++;
            if (begin == nullptr){
                pNode<T> new_node  = new Node<T>(next, idx, value);
                TRACE(new_node, "new");
                return new_node;
            } else {
                pNode<T> new_node = begin;
                begin = begin->next();
                node_counter--;
                new_node->emplace(next, idx, value);
                TRACE(new_node, "old");
                return new_node;
            }
        }

        Idx size()const {
            return node_counter; 
        }

        bool full()const {
            return (balance_counter == 0); 
        }

        void fixSize(){
            clear();
            items.resize(node_counter);
            for (size_t i=0; i<node_counter; i++){
                items[i].setNext(begin);
                begin = &items[i];
            }
            fixed = true;
        }

    private:
        pNode<T> begin = nullptr;
        size_t node_counter;
        size_t balance_counter;
        vector<Node<T> > items;
        bool fixed = false;

        void clear(){
            ASSERT_EQ(balance_counter, 0);
            ASSERT_EQ(fixed, false);
            while (begin != nullptr){
                pNode<T> tmp = begin;
                begin = begin->next();
                delete tmp;
            }
        }
};


}

#endif
