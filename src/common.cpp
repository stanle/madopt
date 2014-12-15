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
#include "common.hpp"

std::string MadOpt::doubleToString(const double a_value, int p){
    std::stringstream ss;
    ss << std::fixed << std::setprecision(p) << a_value;
    std::string s = ss.str();                                    
    s.erase(s.find_last_not_of('0') + 1, std::string::npos);     
    return (s[s.size()-1] == '.') ? s.substr(0, s.size()-1) : s;
}

std::string MadOpt::to_string(PII p){
    return "(" + std::to_string(p.first) + " " + std::to_string(p.second) + ")";
}

std::string MadOpt::to_string(Idx p){
    return std::to_string(p);
}
/* ex: set tabstop=4 shiftwidth=4 expandtab: */
