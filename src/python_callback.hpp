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
#ifndef MADOPT_PYTHON_CALLBACK_H
#define MADOPT_PYTHON_CALLBACK_H

#include "constraint_interface.hpp"

namespace MadOpt {

  class PythonCallback: public ConstraintInterface {
    public:
      typedef double (*g_type)(void *param, void *g_data);

      PythonCallback(g_type, void *g_data, double _lb, double _ub);

      double lb();
      void lb(double v);
      double ub();
      void ub(double v);

      Idx getNNZ_Jac() = 0;
      void getNZ_Jac(unsigned int* jCol) = 0;
      void setEvals(CStack&){}
      const double& getG()const = 0;
      const vector<double>& getJac()const = 0;
      void eval_h(double* values, const double& lambda) = 0;

  private:
    double _lb;
    double _ub;

    g_type g;
    void* g_data;

  };
}
#endif
