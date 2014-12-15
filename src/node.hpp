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

#ifndef MADOPT_NODE_H
#define MADOPT_NODE_H

namespace MadOpt {

template<class T>
class Node;

template<class T>
using pNode = Node<T>*;

template<class T>
class Node{
    public:
        Node(){}

        Node(pNode<T> n, T& id, double& v): idx(id), value(v), _next(n){}

        void setNext(pNode<T> _next){
            this->_next = _next; 
        }

        const pNode<T>& next()const {
            return _next; 
        }

        bool operator<(const Node<T>& other){
            return idx < other.idx; 
        }

        void emplace(pNode<T> n, T& id, double& v){ 
            _next = n; 
            idx = id;
            value = v;
        }

        string toString()const {
            return to_string(idx) + "=" + doubleToString(value);
        }

        T idx;
        double value=0;

    protected:
        pNode<T> _next=nullptr;
};

}

#endif
