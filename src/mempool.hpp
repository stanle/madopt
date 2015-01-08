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
        MemPool(Idx initsize=0): 
            node_counter(initsize), 
            balance_counter(0)
        {
            optimizeAlignment();
        } 

        ~MemPool(){
            clear();
        }

        void setUnused(pNode<T> node){ 
            ASSERT(!node->unused, node, (long)node);
            DEBUG_CODE(node->unused = true);
            TRACE(node, "|", ((long)node));
            node->setNext(begin);
            begin = node;
            balance_counter--;
            node_counter++;
        }

        pNode<T> getNewNode(pNode<T> next, T& idx, double& value){ 
            balance_counter++;
            if (begin == nullptr){
                pNode<T> new_node  = new Node<T>(next, idx, value);
                TRACE(new_node, (long)new_node, "new");
                DEBUG_CODE(new_node->unused = false);
                return new_node;
            } else {
                pNode<T> new_node = begin;
                ASSERT(new_node->unused);
                begin = begin->next();
                node_counter--;
                new_node->emplace(next, idx, value);
                TRACE(new_node, (long)new_node, "old");
                DEBUG_CODE(new_node->unused = false);
                return new_node;
            }
        }

        Idx size()const {
            return node_counter; 
        }

        bool full()const {
            return (balance_counter == 0); 
        }

        void optimizeAlignment(){
            ASSERT_EQ(balance_counter, 0);
            if (node_counter > items.size()){
                auto _size = node_counter;
                clear();
                node_counter = _size;
                items.resize(node_counter);

                pNode<T> frst = itemsFirst();
                pNode<T> lst = itemsLast();

                for (auto& node: items){
                    node.setNext(begin);
                    begin = &node;
                    if (ASSERT_ENABLED){
                        ASSERT(begin >= frst, begin, frst);
                        ASSERT(begin <= lst, begin, lst);
                    }
                }
            }
        }

    private:
        pNode<T> begin = nullptr;
        size_t node_counter;
        size_t balance_counter;
        vector<Node<T> > items;

        void clear(){
            ASSERT_EQ(balance_counter, 0);
            if (items.empty()){
                while (begin != nullptr){
                    pNode<T> tmp = begin;
                    begin = begin->next();
                    delete tmp;
                }
            } else {
                pNode<T> frst = itemsFirst();
                pNode<T> lst = itemsLast();
                while (begin != nullptr){
                    if (begin >= frst && begin <= lst){
                        begin = begin->next();
                    } else{
                        pNode<T> tmp = begin;
                        begin = begin->next();
                        delete tmp;
                    }
                }
                items.clear();
            }
            node_counter = 0;
        }

        inline pNode<T> itemsFirst() {
            return &(items.data()[0]);
        }

        inline pNode<T> itemsLast() {
            return &(items.data()[items.size()-1]);
        }
};

}

#endif
