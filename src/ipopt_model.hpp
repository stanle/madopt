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
#ifndef MADOPT_IPOPT_MODEL_H
#define MADOPT_IPOPT_MODEL_H

#include "model.hpp"

namespace MadOpt {

struct IpoptModelImpl;

//! Model for the NLP solver IPOPT
class IpoptModel: public Model {
    public:
        IpoptModel();
        ~IpoptModel();

  IpoptModel(IpoptModel const &) = delete;
  IpoptModel(IpoptModel&&) = delete;

        void setStringOption(std::string key, std::string value);
        void setNumericOption(std::string key, double value);
        void setIntegerOption(std::string key, int value);

        void solve();

    private:
        IpoptModelImpl* impl;
};

}
#endif
