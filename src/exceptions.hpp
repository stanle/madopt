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
#ifndef MADOPT_EXCEPTIONS_H
#define MADOPT_EXCEPTIONS_H

#include "common.hpp"

#include <exception>
#include <string>

namespace MadOpt {

using namespace std;

class MadOptError: public runtime_error{
    public:
        MadOptError(const string& t): runtime_error(t){}
};
}
#endif
/* ex: set tabstop=4 shiftwidth=4 expandtab: */
