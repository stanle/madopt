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
#ifndef MADOPT_BONMIN_MINLP_H
#define MADOPT_BONMIN_MINLP_H

#include <coin/BonCbc.hpp>
#include <coin/BonBonminSetup.hpp>
#include <coin/BonTMINLP.hpp>

typedef Ipopt::Number Number;
typedef Ipopt::Index Index;

namespace MadOpt {

class BonminModel;

class BonminUserClass : public Bonmin::TMINLP {
public:
  BonminUserClass(BonminModel* solver);

  ~BonminUserClass(){}

  /** Method to return some info about the nlp */
  virtual bool get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
                            Index& nnz_h_lag, Ipopt::TNLP::IndexStyleEnum& index_style);

  /** Method to return the bounds for my problem */
  virtual bool get_bounds_info(Index n, Number* x_l, Number* x_u,
                               Index m, Number* g_l, Number* g_u);

  /** Method to return the starting point for the algorithm */
  virtual bool get_starting_point(Index n, bool init_x, Number* x,
                                  bool init_z, Number* z_L, Number* z_U,
                                  Index m, bool init_lambda,

                                  Number* lambda);

  /** Method to return the objective value */
  virtual bool eval_f(Index n, const Number* x, bool new_x, Number& obj_value);

  /** Method to return the gradient of the objective */
  virtual bool eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f);

  /** Method to return the constraint residuals */
  virtual bool eval_g(Index n, const Number* x, bool new_x, Index m, Number* g);

  /** Method to return:
   *   1) The structure of the jacobian (if "values" is NULL)
   *   2) The values of the jacobian (if "values" is not NULL)
   */
  virtual bool eval_jac_g(Index n, const Number* x, bool new_x,
                          Index m, Index nele_jac, Index* iRow, Index *jCol,
                          Number* values);

  /** Method to return:
   *   1) The structure of the hessian of the lagrangian (if "values" is NULL)
   *   2) The values of the hessian of the lagrangian (if "values" is not NULL)
   */
  virtual bool eval_h(Index n, const Number* x, bool new_x,
                      Number obj_factor, Index m, const Number* lambda,
                      bool new_lambda, Index nele_hess, Index* iRow,
                      Index* jCol, Number* values);

  virtual void finalize_solution(SolverReturn status, Index n, const Number* x, Number obj_value);
  virtual bool get_variables_types(Index n, Bonmin::TMINLP::VariableType* var_types); 

  virtual bool get_variables_linearity(Index n, Ipopt::TNLP::LinearityType* var_types);

  virtual bool get_constraints_linearity(Index m, Ipopt::TNLP::LinearityType* const_types){
    // for (int i=0; i<m; i++)
      // const_types[i] = Ipopt::TNLP::LINEAR;
    // return true;
      return false;
  }

  virtual const SosInfo * sosConstraints() const{return NULL;}
  virtual const BranchingInfo* branchingInfo() const{return NULL;}

private:
  BonminModel* solver;
};

}
#endif
/* ex: set tabstop=4 shiftwidth=4 expandtab: */
