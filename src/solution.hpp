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
#ifndef MADOPT_SOLUTION_H
#define MADOPT_SOLUTION_H

//#include <vector>
#include "common.hpp"

namespace MadOpt {

//! solution class
class Solution{
    public:

        //! enum for solver status, copied from ipopt
        enum SolverStatus {
            SUCCESS,
            MAXITER_EXCEEDED,
            CPUTIME_EXCEEDED,
            STOP_AT_TINY_STEP,
            STOP_AT_ACCEPTABLE_POINT,
            LOCAL_INFEASIBILITY,
            USER_REQUESTED_STOP,
            FEASIBLE_POINT_FOUND,
            DIVERGING_ITERATES,
            RESTORATION_FAILURE,
            ERROR_IN_STEP_COMPUTATION,
            INVALID_NUMBER_DETECTED,
            TOO_FEW_DEGREES_OF_FREEDOM,
            INVALID_OPTION,
            OUT_OF_MEMORY,
            INTERNAL_ERROR,
            UNASSIGNED,
            NO_RUN=-1
        };

        void set(const SolverStatus status,
                const Idx x_size, const Idx l_size, 
                const double obj_value, 
                const double* x, const double* lambda);

        void set(const SolverStatus status,
                const Idx x_size, 
                const double obj_value, 
                const double* x);

        //! number of variables
        Idx nx()const ;

        //! number of constraints
        Idx ng()const ;

        double x(const Idx idx)const ;

        double v(const Idx idx)const ;

        double lam(const Idx idx)const ;

        //! solver status
        SolverStatus status()const ;

        //! objective value
        double obj_value()const ;

        //! returns true if a solution is loaded
        bool hasSolution() const;

    private:
        vector<double> _x;
        vector<double> _l;
        double _obj_value;
        SolverStatus _status = SolverStatus::NO_RUN;
        bool lambda_loaded=false;
};

}
#endif
