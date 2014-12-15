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
#include "../src/bonmin_model.hpp"
#include "../src/exceptions.hpp"

using namespace MadOpt;

class BonminModelTest: public CxxTest::TestSuite {
    public:

        void testSimple(){
            BonminModel m;
            Var y = m.addVar(2, 10, 5, "x");
            Var x1 = m.addVar(4,100, 4, "x1");

            m.addEqConstr(y - x1, 0);
            m.addEqConstr(y*y - x1, 0);

            m.setObj(Expr(0));
            m.solve();
        }

};
