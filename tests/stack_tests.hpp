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
#include <cxxtest/TestSuite.h>
#include <vector>
#include "testmodel.hpp"
using namespace std;
using namespace MadOpt;

class ADStackTest: public CxxTest::TestSuite {
    public:

        typedef pNode<Idx> pJacNode;

        void Tes(vector<Idx> l1idx, vector<double> l1val, vector<Idx> l2idx, vector<double> l2val,
                vector<Idx> residx, vector<double> resval, double m1=1, double m2=1){
            JacMemPool jmp(l1idx.size() + l2idx.size());
            JacList l1(jmp), l2(jmp);

            for (size_t i=0; i<l1idx.size(); i++)
                l1.emplace_front(l1idx[i], l1val[i]);

            for (size_t i=0; i<l2idx.size(); i++)
                l2.emplace_front(l2idx[i], l2val[i]);

            l1.mergeInto(l2, m1, m2);

            TS_ASSERT_EQUALS(resval.size(), l1.size());

            int i = 0;
            for (auto n=l1.begin(); n!=l1.end(); n=n->next()){
                TS_ASSERT_EQUALS(residx[i], n->idx);
                TS_ASSERT_EQUALS(resval[i], n->value);
                i++;
            }
        }

        void Tes(vector<Idx> l1idx, vector<double> l1val, vector<Idx> l2idx, vector<double> l2val, vector<PII> hidx, vector<double> hval,
                vector<PII> residx, vector<double> resval, double m1=1, double m2=1){
            JacMemPool jmp(l1idx.size() + l2idx.size());
            HessMemPool hmp(residx.size());
            JacList l1(jmp), l2(jmp);

            for (size_t i=0; i<l1idx.size(); i++)
                l1.emplace_front(l1idx[i], l1val[i]);

            for (size_t i=0; i<l2idx.size(); i++)
                l2.emplace_front(l2idx[i], l2val[i]);

            HessList hl(hmp);
            for (size_t i=0; i<hidx.size(); i++)
                hl.emplace_front(hidx[i], hval[i]);

            hl.mergeInto(l1, l2, m1, m2);

            TS_ASSERT_EQUALS(resval.size(), hl.size());

            int i = 0;
            for (auto n=hl.begin(); n!=hl.end(); n=n->next()){
                TS_ASSERT_EQUALS(residx[i], n->idx);
                TS_ASSERT_EQUALS(resval[i], n->value);
                i++;
            }
        }

        void testMemPoolSize(){
            JacMemPool p(23);
            TS_ASSERT_EQUALS(p.capacity(), 23);
            TS_ASSERT_EQUALS(p.size(), 23);

            Idx i = 1;
            double v = 1;
            pJacNode s = p.getNewNode(nullptr, i, v);

            TS_ASSERT_EQUALS(p.capacity(), 23);
            TS_ASSERT_EQUALS(p.size(), 22);

            p.setUnused(s);

            TS_ASSERT_EQUALS(p.capacity(), 23);
            TS_ASSERT_EQUALS(p.size(), 23);

            p.reserve(12);

            TS_ASSERT_EQUALS(p.capacity(), 23);
            TS_ASSERT_EQUALS(p.size(), 23);

            p.reserve(45);

            TS_ASSERT_EQUALS(p.capacity(), 45);
            TS_ASSERT_EQUALS(p.size(), 45);
        }

        void testCreate(){
             JacMemPool p(3);
             JacList l(p);
             l.emplace_front(1, 1);
        }
        
        void testEmplaceFront(){
             JacMemPool p(4);
             TS_ASSERT_EQUALS(p.capacity(), 4);
             JacList l(p);
             l.emplace_front(4, 4);
             l.emplace_front(3, 3);
             l.emplace_front(2, 2);
             l.emplace_front(1, 1);
             TS_ASSERT_EQUALS(p.size(), 0);
             TS_ASSERT_EQUALS(p.capacity(), 4);
             int i=1;
             for (auto node=l.begin(); node != l.end(); node = node->next()){
                 TS_ASSERT_EQUALS(node->idx, i);
                 TS_ASSERT_EQUALS(node->value, (double)i);
                 i++;
             }
        }

        void testMulAll(){
             JacMemPool p(4);
             JacList l(p);
             l.emplace_front(4, 4);
             l.emplace_front(3, 3);
             l.emplace_front(2, 2);
             l.emplace_front(1, 1);
             TS_ASSERT_EQUALS(p.size(), 0);
             TS_ASSERT_EQUALS(p.capacity(), 4);
             l.mulAll(3);
             int i=1;
             for (auto node=l.begin(); node != l.end(); node = node->next()){
                 TS_ASSERT_EQUALS(node->idx, i);
                 TS_ASSERT_EQUALS(node->value, 3*(double)i);
                 i++;
             }
             TS_ASSERT_EQUALS(p.capacity(), 4);
        }

        void testMergeInto(){
            Tes({5,4,2,1}, {4,4,1,1}, {5,3,2}, {1,3,1}, {1,2,3,4,5}, {1,2,3,4,5});

            Tes({}, {}, {2,1}, {1,1}, {1,2}, {3,3}, 1, 3);

            Tes({2,1}, {1,1}, {}, {}, {1,2}, {3,3}, 3, 1);
        }

       void testMergeIntoHess(){
           Tes({3,2,1}, {3,2,1}, {2,1}, {4,3}, {PII(1,3), PII(1,1)}, {1,1},
                   {PII(1,1), PII(1,2), PII(1,3), PII(2,2)}, {4,4,1,8});

           Tes({1}, {2}, {2,1}, {4,3}, {}, {}, {PII(1,1), PII(1,2)}, {6,8});

           Tes({1}, {1}, {1}, {3}, {PII(1,3), PII(1,1)}, {1,1}, {PII(1,1), PII(1,3)}, {4,1});

           Tes({}, {}, {}, {}, {PII(2,2), PII(1,1)}, {1,1}, {PII(1,1), PII(2,2)}, {3,3}, 3, 1);

           Tes({1}, {1}, {2,1}, {1,1}, {}, {}, {PII(1,1), PII(1,2)}, {3,3}, 1, 3);
       }

       void testEmpty(){
           ADStack stack;

           TS_ASSERT_EQUALS(stack.capacity(), 0);
           TS_ASSERT_EQUALS(stack.empty(), true);

           stack.emplace_back(12);
           stack.emplace_back(12);

           TS_ASSERT_EQUALS(stack.capacity(), 2);
           TS_ASSERT_EQUALS(stack.empty(), false);

           stack.pop_back();

           TS_ASSERT_EQUALS(stack.capacity(), 2);
           TS_ASSERT_EQUALS(stack.size(), 1);
           TS_ASSERT_EQUALS(stack.empty(), false);

           stack.pop_back();

           TS_ASSERT_EQUALS(stack.capacity(), 2);
           TS_ASSERT_EQUALS(stack.size(), 0);
           TS_ASSERT_EQUALS(stack.empty(), true);
       }

       void testReserve(){
           ADStack stack(1, 2, 3);

           TS_ASSERT_EQUALS(stack.capacity(), 1);
           TS_ASSERT_EQUALS(stack.jac_mempool_capacity(), 2);
           TS_ASSERT_EQUALS(stack.hess_mempool_capacity(), 3);

           stack.reserve(10, 20, 30);

           TS_ASSERT_EQUALS(stack.capacity(), 10);
           TS_ASSERT_EQUALS(stack.jac_mempool_capacity(), 20);
           TS_ASSERT_EQUALS(stack.hess_mempool_capacity(), 30);

           stack.reserve(1, 2, 3);

           TS_ASSERT_EQUALS(stack.capacity(), 10);
           TS_ASSERT_EQUALS(stack.jac_mempool_capacity(), 20);
           TS_ASSERT_EQUALS(stack.hess_mempool_capacity(), 30);
       }
};
