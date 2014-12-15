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
#ifndef MADOPT_INNER_PARAM_H
#define MADOPT_INNER_PARAM_H

namespace MadOpt {

class InnerParam{
    public:
        InnerParam(const double v, const string& n): _value(v), _name(n){}

        void value(const double v){ _value = v; }

        double value()const { return _value; }

        string name()const { return _name;}

    private:
        double _value;
        string _name;
};

}
#endif
