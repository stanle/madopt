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
#ifndef MADOPT_COMMON_H
#define MADOPT_COMMON_H

#include <assert.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <sstream>

namespace std {
    template <class T>
    inline void hash_combine(std::size_t & seed, const T & v){
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template<typename S, typename T> struct hash<pair<S, T>>{
        inline size_t operator()(const pair<S, T> & v) const {
            size_t seed = 0;
            hash_combine(seed, v.first);
            hash_combine(seed, v.second);
            return seed;
        }
    };

    template<typename T>
    std::ostream& operator << ( std::ostream& out, 
            const std::pair<T, T>& rhs ) {
        out << rhs.first << ", " << rhs.second;
        return out;
    }
}

namespace MadOpt {

enum VarType {
    CONTINUOUS,
    INTEGER,
    BINARY
};

using namespace std;
const double INF = std::numeric_limits<double>::infinity();

string doubleToString(const double a_value, int p=10);

typedef unsigned int Idx;
typedef pair<Idx, Idx> PII;
#define uPII(a,b) (((a)<(b))?(PII((a),(b))):(PII((b),(a))))
typedef unordered_map<PII, int> HessPosMap;
typedef vector<double> ParamData;

string to_string(PII p);
string to_string(Idx p);
string to_string(vector<Idx> v);
string to_string(vector<PII> v);
string to_string(vector<double> v);

#define FOREACH(elem, c) for(auto __it=c.begin(); __it!=c.end(); __it++){ auto& elem = *__it;

}

#endif
/* ex: set tabstop=4 shiftwidth=4 expandtab: */
