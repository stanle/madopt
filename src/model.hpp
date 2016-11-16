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
#ifndef MADOPT_MODEL_H
#define MADOPT_MODEL_H

#include "common.hpp"

#include "cstack.hpp"
#include "simstack.hpp"
#include "var.hpp"
#include "param.hpp"
#include "constraint.hpp"
#include "solution.hpp"
#include "constraint_interface.hpp"

namespace MadOpt {

//class ThreadPool;

//! generic Model class, not for direct use hence the constructor is protected
class Model {
    public:
        Model(): show_solver(false), timelimit(-1), model_changed(false),
                 obj(new InnerConstraint(Expr(0), 0, 0, hess_pos_map, simstack)){}

  Model(Model const &) = delete;
  Model(Model&&) = delete;

        virtual ~Model();

        //! starts the solver
        virtual void solve()=0; 

        // Options 

        //! set string option, the options depend on the solver, this method
        //simply passes the options to the solver
        virtual void setStringOption(std::string key, std::string value){}

        //! set numeric option, the options depend on the solver, this method
        //simply passes the options to the solver
        virtual void setNumericOption(std::string key, double value){}

        //! set integer option, the options depend on the solver, this method
        //simply passes the options to the solver
        virtual void setIntegerOption(std::string key, int value){}

        // Var stuff
        
        //! \sa addVar(double, double, double, string)
        Var addVar(string name);

        /*! \brief add a new continuous variable to the model,
         * \details
         * @param[in] lb lower bound
         * @param[in] ub upper bound
         * @param[in] init initial value
         * @param[in] name name of the variable
         */
        Var addVar(double lb, double ub, double init, string name);

        //! \sa addVar(double, double, double, string)
        Var addVar(double lb, double ub, string name);

        //! \sa addVar(double, double, double, string)
        Var addVar(double init, string name);

        //! \sa addVar(double, double, double, string)
        Var addCVar(double lb, double ub, double init, string name);

        //! \sa addVar(double, double, double, string)
        Var addCVar(double lb, double ub, string name);

        /*! \brief add a new integer variable to the model,
         * \details
         * @param[in] lb lower bound
         * @param[in] ub upper bound
         * @param[in] init initial value
         * @param[in] name name of the variable
         */
        Var addIVar(double lb, double ub, double init, string name);

        //! \sa addIVar()
        Var addIVar(double lb, double ub, string name);

        /*! \brief add a new binary variable to the model,
         * @param[in] init initial value
         * @param[in] name name of the variable
         */
        Var addBVar(double init, string name);

        /*! \brief add a new binary variable to the model,
         * @param[in] name name of the variable
         */
        Var addBVar(string name);

        //Param stuff
        /*! \brief add a new parameter to the model,
         * @param[in] value value
         * @param[in] name name of the variable
         */
        Param addParam(double value, string name);

        //Constraint stuff
        /*! add new constraint that is based on Expr, lb <= expr <= ub
         * \param[in] expr the constraint expression
         * \param[in] ub the upper bound of the constraint
         * \param[in] lb the lower bound of the constraint
         */
        Constraint addConstr(const double lb, const Expr& expr, double ub);

        /*! add new equality constraint that is based on Expr, expr == equal
         * \param[in] expr the constraint expression
         * \param[in] equal the value the constraint is equal to
         */
        Constraint addEqConstr(const Expr& expr, const double equal);

        /*! add new upper bounded only constraint that is based on Expr, expr
         * <= ub
         * \param[in] expr the constraint expression
         * \param[in] ub the upper bound of the constraint
         */
        Constraint addConstr(const Expr& expr, const double ub);

        /*! add new lower bounded only constraint that is based on Expr, lb <=
         * expr
         * \param[in] expr the constraint expression
         * \param[in] lb the lower bound of the constraint
         */
        Constraint addConstr(const double lb, const Expr& expr);

        /*! add new custom constraint
        * expr
        * \param[in] pointer to custom constraint, do not del mem on your own 
        */
        Constraint addConstr(ConstraintInterface* con);

        //Objective Stuff
        /*! set objective based on custom objective implementation that is
         * derived from InnerConstraint
         */
        void setObj(ConstraintInterface* constraint);

        //! set objective based on Expr 
        void setObj(const Expr& expr);

        //NLP init stuff
        Idx getNNZ_Jac();
        Idx getNNZ_Hess();
        void getNZ_Jac(int* iRow, int* jCol);
        void getNZ_Hess(int* iRow, int* jCol);
        void getBounds(double* xl, double* xu, double* gl, double* gu);
        void getInits(double* xi);

        //! set the currently loaded solution as initial values, 
        void solAsInit();

        //! number of variables
        Idx nx() const;

        //! number of constraints
        Idx ng() const;

        //! number of parameters
        Idx np() const;

        // Eval functions
        void setEvals(const double* x);
        void eval_f(const double* x, bool new_x, double& obj_value);
        void eval_grad_f(const double* x, bool new_x, double* grad_f);
        void eval_g(const double* x, bool new_x, double* g);
        void eval_jac_g(const double* x, bool new_x, double* values);
        void eval_h(const double* x, bool new_x, double* values,
                double obj_factor, const double* lambda);

        //! objective value 
        double objValue() const;

        //! solution status
        Solution::SolverStatus status() const;

        //! \brief true if a solution is loaded, 
        // \details should be checked after calling solve() before accessing
        // any solution values
        bool hasSolution() const;

        //! returns the solution object that is currently loaded
        Solution& getSolution(){ return solution; }

        //! enable/disable printing options of the solver, Overwrites! the
        //options
        bool show_solver;

        //! timelimit, a negative value is interpreted as no time limit
        double timelimit;

        const vector<InnerVar*>& getVars()const { return vars; }

        double lb(Idx idx) const;
        void lb(Idx idx, double v);

        double ub(Idx idx) const;
        void ub(Idx idx, double v);

        const string toString()const;

        SimStack& getSimStack(){ return simstack; }
        CStack& getCStack(){ return cstack; }

    protected:
        bool model_changed;
        vector<InnerVar*> vars;

    private:
        vector<InnerParam*> params;
        vector<ConstraintInterface*> constraints;
        CStack cstack;
        SimStack simstack;
        ConstraintInterface* obj;
        vector<Idx> obj_jac_map;
        HessPosMap hess_pos_map;
        Solution solution;

        Var addVar(double lb, double ub, VarType type, double init, string name);
};
}
#endif
/* ex: set tabstop=4 shiftwidth=4 expandtab: */
