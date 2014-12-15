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
#include "bonmin_minlp.hpp"
#include "bonmin_model.hpp"
#include "solution.hpp"

using namespace MadOpt;

BonminUserClass::BonminUserClass(BonminModel* solver): solver(solver){}

bool BonminUserClass::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
                            Index& nnz_h_lag, Ipopt::TNLP::IndexStyleEnum& index_style){
    n = solver->nx();
    m = solver->ng();
    nnz_jac_g = solver->getNNZ_Jac();
    nnz_h_lag = solver->getNNZ_Hess();
    index_style = Ipopt::TNLP::C_STYLE;
    return true;
}

bool BonminUserClass::get_variables_types(Index n, Bonmin::TMINLP::VariableType* var_types){
    auto vars = solver->getVars();
    for (Idx i=0; i<solver->nx(); i++){
        VarType vt = vars[i]->getType();
        if (vt == VarType::CONTINUOUS)
            var_types[i] = Bonmin::TMINLP::VariableType::CONTINUOUS;
        else if (vt == VarType::BINARY)
            var_types[i] = Bonmin::TMINLP::VariableType::BINARY;
        else if (vt == VarType::INTEGER)
            var_types[i] = Bonmin::TMINLP::VariableType::INTEGER;
        else
            assert(false);
    }
    return true;
  }

void BonminUserClass::finalize_solution(SolverReturn status, Index n, const Number* x, Number obj_value){
    Solution& sol = solver->getSolution();
    Solution::SolverStatus s;
    switch(status){
        case SolverReturn::SUCCESS:
            s = Solution::SolverStatus::SUCCESS;
            break;

        case SolverReturn::INFEASIBLE:
            s = Solution::SolverStatus::LOCAL_INFEASIBILITY;
            break;

        case SolverReturn::CONTINUOUS_UNBOUNDED:
            s = Solution::SolverStatus::LOCAL_INFEASIBILITY;
            break;

        case SolverReturn::LIMIT_EXCEEDED:
            s = Solution::SolverStatus::CPUTIME_EXCEEDED;
            break;

        case SolverReturn::USER_INTERRUPT:
            s = Solution::SolverStatus::USER_REQUESTED_STOP;
            break;

        case SolverReturn::MINLP_ERROR:
            s = Solution::SolverStatus::INTERNAL_ERROR;
            break;

        default:
           throw MadOptError("Unknown Bonmin status");
    }
    sol.set(s, n, obj_value, x);
}

bool BonminUserClass::get_bounds_info(Index n, Number* x_l, Number* x_u,
                               Index m, Number* g_l, Number* g_u){
    assert(n==solver->nx());
    assert(m==solver->ng());
    solver->getBounds(x_l, x_u, g_l, g_u);
    return true;
  }

bool BonminUserClass::get_starting_point(Index n, bool init_x, Number* x,
                                  bool init_z, Number* z_L, Number* z_U,
                                  Index m, bool init_lambda,
                                  Number* lambda){
    assert(n==solver->nx());
    if (init_x)
        solver->getInits(x);
    return true;
  }

bool BonminUserClass::eval_f(Index n, const Number* x, bool new_x, Number& obj_value){
    assert(n==solver->nx());
    solver->eval_f(x, new_x, obj_value);
    return true;
}

bool BonminUserClass::eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f){
    assert(n==solver->nx());
    solver->eval_grad_f(x, new_x, grad_f);
    return true;
}

bool BonminUserClass::eval_g(Index n, const Number* x, bool new_x, Index m, Number* g){
    assert(n==solver->nx());
    solver->eval_g(x, new_x, g);
    return true;
}

bool BonminUserClass::eval_jac_g(Index n, const Number* x, bool new_x,
                        Index m, Index nele_jac, Index* iRow, Index *jCol,
                        Number* values){
    assert(n==solver->nx());
    assert(m==solver->ng());
    if (values == NULL){
        solver->getNZ_Jac(iRow, jCol);
    } else {
        solver->eval_jac_g(x, new_x, values);
    }
    return true;
}

bool BonminUserClass::eval_h(Index n, const Number* x, bool new_x,
                    Number obj_factor, Index m, const Number* lambda,
                    bool new_lambda, Index nele_hess, Index* iRow,
                    Index* jCol, Number* values){
    assert(n==solver->nx());
    assert(m==solver->ng());
    if (values == NULL){
        solver->getNZ_Hess(iRow, jCol);
    } else {
        solver->eval_h(x, new_x, values, obj_factor, lambda);
    }
    return true;
}

bool BonminUserClass::get_variables_linearity(Index n, Ipopt::TNLP::LinearityType* var_types){
//      var_types[i] = Ipopt::TNLP::NON_LINEAR;
//      var_types[i] = Ipopt::TNLP::LINEAR;
  return false;
}
/* ex: set tabstop=4 shiftwidth=4 expandtab: */
