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
#include "ipopt_nlp.hpp"

#include "logger.hpp"
#include "model.hpp"

using namespace MadOpt;

bool IpoptUserClass::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
                            Index& nnz_h_lag, Ipopt::TNLP::IndexStyleEnum& index_style){
    TRACE_START;
    n = solver->nx();
    m = solver->ng();
    nnz_jac_g = solver->getNNZ_Jac();
    nnz_h_lag = solver->getNNZ_Hess();
    index_style = Ipopt::TNLP::C_STYLE;
    VALGRIND_CONDITIONAL_JUMP_TEST(n);
    VALGRIND_CONDITIONAL_JUMP_TEST(m);
    VALGRIND_CONDITIONAL_JUMP_TEST(nnz_jac_g);
    VALGRIND_CONDITIONAL_JUMP_TEST(nnz_h_lag);
    TRACE_END;
    return true;
}

void IpoptUserClass::finalize_solution(
    Ipopt::SolverReturn               status    ,
    Index                             n         , 
    const Number*                     x         , 
    const Number*                     z_L       , 
    const Number*                     z_U       ,
    Index                             m         , 
    const Number*                     g         , 
    const Number*                     lambda    ,
    Number                            obj_value ,
    const Ipopt::IpoptData*           ip_data   ,
    Ipopt::IpoptCalculatedQuantities* ip_cq
){
    TRACE_START;
    Solution& sol = solver->getSolution();
    Solution::SolverStatus s = (Solution::SolverStatus)status;
    sol.set(s, n, m, obj_value, x, lambda);
    TRACE_END;
}

bool IpoptUserClass::get_bounds_info(Index n, Number* x_l, Number* x_u,
                               Index m, Number* g_l, Number* g_u){
    TRACE_START;
    assert((Idx)n==solver->nx());
    assert((Idx)m==solver->ng());
    solver->getBounds(x_l, x_u, g_l, g_u);
    VALGRIND_CONDITIONAL_JUMP_TEST_LOOP(n, x_l);
    VALGRIND_CONDITIONAL_JUMP_TEST_LOOP(n, x_u);
    VALGRIND_CONDITIONAL_JUMP_TEST_LOOP(m, g_u);
    VALGRIND_CONDITIONAL_JUMP_TEST_LOOP(m, g_l);
    TRACE_END;
    return true;
  }

bool IpoptUserClass::get_starting_point(Index n, bool init_x, Number* x,
                                  bool init_z, Number* z_L, Number* z_U,
                                  Index m, bool init_lambda,
                                  Number* lambda){
    TRACE_START;
    assert((Idx)n==solver->nx());
    if (init_x)
        solver->getInits(x);
        VALGRIND_CONDITIONAL_JUMP_TEST_LOOP(n, x);
    if (init_z)
        throw MadOptError("no dual init implemented");
    if (init_lambda)
        throw MadOptError("no lambda init implemented");
    TRACE_END;
    return true;
}

bool IpoptUserClass::eval_f(Index n, const Number* x, bool new_x, Number& obj_value){
    TRACE_START;
    assert((Idx)n==solver->nx());
    solver->eval_f(x, new_x, obj_value);
    VALGRIND_CONDITIONAL_JUMP_TEST(obj_value);
    TRACE_END;
    return true;
}

bool IpoptUserClass::eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f){
    TRACE_START;
    TRACE("new_x=", new_x);
    assert((Idx)n==solver->nx());
    solver->eval_grad_f(x, new_x, grad_f);
    VALGRIND_CONDITIONAL_JUMP_TEST_LOOP(n, grad_f);
    TRACE_END;
    return true;
}

bool IpoptUserClass::eval_g(Index n, const Number* x, bool new_x, Index m, Number* g){
    TRACE_START;
    TRACE("new_x=", new_x);
    assert((Idx)n==solver->nx());
    solver->eval_g(x, new_x, g);
    VALGRIND_CONDITIONAL_JUMP_TEST_LOOP(m, g);
    TRACE_END;
    return true;
}

bool IpoptUserClass::eval_jac_g(Index n, const Number* x, bool new_x,
                        Index m, Index nele_jac, Index* iRow, Index *jCol,
                        Number* values){
    TRACE_START;
    TRACE("new_x=", new_x);
    assert((Idx)n==solver->nx());
    assert((Idx)m==solver->ng());
    if (values == NULL){
        TRACE("get sparse Jacobean entries");
        solver->getNZ_Jac(iRow, jCol);
        VALGRIND_CONDITIONAL_JUMP_TEST_LOOP(nele_jac, jCol);
        VALGRIND_CONDITIONAL_JUMP_TEST_LOOP(nele_jac, iRow);
    } else {
        solver->eval_jac_g(x, new_x, values);
        VALGRIND_CONDITIONAL_JUMP_TEST_LOOP(nele_jac, values);
    }
    TRACE_END;
    return true;
}

bool IpoptUserClass::eval_h(Index n, const Number* x, bool new_x,
                    Number obj_factor, Index m, const Number* lambda,
                    bool new_lambda, Index nele_hess, Index* iRow,
                    Index* jCol, Number* values){
    TRACE_START;
    TRACE("new_x=", new_x);
    assert((Idx)n==solver->nx());
    assert((Idx)m==solver->ng());
    assert((Idx)nele_hess==solver->getNNZ_Hess());
    if (values == NULL){
        TRACE("get sparse Hessian entries");
        solver->getNZ_Hess(iRow, jCol);
        VALGRIND_CONDITIONAL_JUMP_TEST_LOOP(nele_hess, jCol);
        VALGRIND_CONDITIONAL_JUMP_TEST_LOOP(nele_hess, iRow);
    } else {
        solver->eval_h(x, new_x, values, obj_factor, lambda);
        VALGRIND_CONDITIONAL_JUMP_TEST_LOOP(nele_hess, values);
    }
    TRACE_END;
    return true;
}

bool IpoptUserClass::get_variables_linearity(Index n, Ipopt::TNLP::LinearityType* var_types){
  return false;
}
/* ex: set tabstop=4 shiftwidth=4 expandtab: */
