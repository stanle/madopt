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

#ifndef MADOPT_ADLIST_H
#define MADOPT_ADLIST_H

#include "node.hpp"
#include "mempool.hpp"

namespace MadOpt {

template<class T>
class ADList: public Node<T>{
    public:
        ADList(MemPool<T>& p): mempool(p){}
        ~ADList(){ clear(); }

        void clear(){
            pNode<T> tmp;
            while (this->_next != end()){
                tmp = this->_next;
                this->_next = this->_next->next();
                mempool.setUnused(tmp);
            }
            ASSERT_EQ(begin(), end());
        }

        pNode<T> begin()const {
            return this->_next; 
        }

        pNode<T> end()const {
            return nullptr; 
        }

        bool empty()const {
            return (begin() == end()); 
        }

        pNode<T> moveBegin(){
            pNode<T> tmp = this->_next;
            this->_next = end();
            return tmp; 
        }

        Idx size()const {
            Idx c(0);
            auto i=begin();
            while (i!=end()) {
                i=i->next();
                c++;
            } 
            return c;
        }

        void emplace_front(T idx, double value){
            pNode<T> tmp = this->_next;
            this->_next = mempool.getNewNode(tmp, idx, value);
        }
        
        void mulAll(double w){
            for (pNode<T> n=begin(); n!=end(); n=n->next())
                n->value *= w;
        }

        void mergeInto(ADList<T>& src){
            pNode<T> dprev = this;
            auto diter = begin();
            auto siter = src.moveBegin();
            pNode<T> tmp;

            while (siter != src.end() && diter != end()){
                if (*diter < *siter){
                    diter = diter->next();
                    dprev = dprev->next();
                } else if (*siter < *diter){
                    dprev->setNext(siter);
                    dprev = dprev->next();
                    siter = siter->next();
                    dprev->setNext(diter);
                } else {
                    diter->value += siter->value;
                    diter = diter->next();
                    dprev = dprev->next();
                    tmp = siter;
                    siter = siter->next();
                    mempool.setUnused(tmp);
                }
            }

            if (siter != src.end())
                dprev->setNext(siter);
        }


        void mergeInto(ADList<T>& src, double wd, double ws){
            pNode<T> dprev = this;
            auto diter = begin();
            auto siter = src.moveBegin();
            pNode<T> tmp;

            while (siter != src.end() && diter != end()){
                if (*diter < *siter){
                    diter->value *= wd;
                    diter = diter->next();
                    dprev = dprev->next();
                } else if (*siter < *diter){
                    siter->value *= ws;
                    dprev->setNext(siter);
                    dprev = dprev->next();
                    siter = siter->next();
                    dprev->setNext(diter);
                } else {
                    diter->value = wd*diter->value + ws*siter->value;
                    diter = diter->next();
                    dprev = dprev->next();
                    tmp = siter;
                    siter = siter->next();
                    mempool.setUnused(tmp);
                }
            }

            if (siter != src.end()){
                dprev->setNext(siter);
                while (siter != src.end()){
                    siter->value *= ws;
                    siter = siter->next();
                }
            } else {
                while (diter != end()){
                    diter->value *= wd;
                    diter = diter->next();
                }
            }
        }

        template<class W>
        void mergeInto(ADList<W>& src1, ADList<W>& src2){
            pNode<T> dprev = this;
            auto diter = begin();
            auto iter = src1.begin();
            auto runner = src2.begin();
            auto pos_store = src2.begin();

            while (iter != src1.end() && pos_store != src2.end()){
                if (*pos_store < *iter){
                    runner = runner->next();
                    pos_store = pos_store->next();
                } else  if (runner == src2.end()){
                    iter = iter->next();
                    pos_store = pos_store->next();
                    runner = pos_store;
                } else {
                    PII p(iter->idx, runner->idx);
                    while (diter != end() && diter->idx < p){
                        diter = diter->next();
                        dprev = dprev->next();
                    }

                    double v = iter->value * runner->value;
                    if (diter == end() || p < diter->idx){
                        dprev->setNext(mempool.getNewNode(dprev->next(), p, v));
                    } else {
                        diter->value += v;
                        diter = diter->next();
                    }
                    runner = runner->next();
                    dprev = dprev->next();
                }
            }
        }

        template<class W>
        void mergeInto(ADList<W>& src1, ADList<W>& src2, double u, double w){
            pNode<T> dprev = this;
            auto diter = begin();
            auto iter = src1.begin();
            auto runner = src2.begin();
            auto pos_store = src2.begin();

            while (iter != src1.end() && pos_store != src2.end()){
                if (*pos_store < *iter){
                    runner = runner->next();
                    pos_store = pos_store->next();
                } else  if (runner == src2.end()){
                    iter = iter->next();
                    pos_store = pos_store->next();
                    runner = pos_store;
                } else {
                    PII p(iter->idx, runner->idx);
                    while (diter != end() && diter->idx < p){
                        diter->value *= u;
                        diter = diter->next();
                        dprev = dprev->next();
                    }

                    double v = w * iter->value * runner->value;
                    if (diter == end() || p < diter->idx){
                        dprev->setNext(mempool.getNewNode(dprev->next(), p, v));
                    } else {
                        diter->value *= u;
                        diter->value += v;
                        diter = diter->next();
                    }
                    runner = runner->next();
                    dprev = dprev->next();
                }
            }

            while (diter != end()){
                diter->value *= u;
                diter = diter->next();
            }
        }

        string toString()const{
            string res;
            for (auto n=begin(); n!=end(); n=n->next()){
                res += n->toString() + " ";
            }
            return res;
        }

    protected:
        MemPool<T>& mempool;
};


}

#endif
