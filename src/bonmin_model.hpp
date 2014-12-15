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
#ifndef MADOPT_BONMIN_MODEL_H
#define MADOPT_BONMIN_MODEL_H

#include "model.hpp"

namespace MadOpt {

struct BonminModelImpl;

//! Model for the MINLP solver Bonmin
class BonminModel: public Model {
    public:
        //! constructs a new BonminModel
        BonminModel();
        ~BonminModel();
        void setStringOption(std::string key, std::string value);
        void setNumericOption(std::string key, double value);
        void setIntegerOption(std::string key, int value);
        void solve();

    private:
        BonminModelImpl* impl;
};

}
#endif
