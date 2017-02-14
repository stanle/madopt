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
#include "bonmin_model.hpp"
#include "bonmin_minlp.hpp"

#include "common.hpp"

using namespace MadOpt;

namespace MadOpt {

struct BonminModelImpl {
    BonminModelImpl(BonminModel* model){
        Bapp = new Bonmin::BonminSetup();
        Bapp->initializeOptionsAndJournalist();
        bonmin_callback = new BonminUserClass(model);
    }

    ~BonminModelImpl(){
        delete Bapp;
    }

    Bonmin::BonminSetup* Bapp;
    Ipopt::SmartPtr<Bonmin::TMINLP> bonmin_callback;
};
}

BonminModel::BonminModel(): Model(){
    impl = new BonminModelImpl(this);
}

BonminModel::~BonminModel(){
    delete impl;
}

void BonminModel::solve(){
    if (timelimit >= 0)
        setNumericOption("bonmin.time_limit", timelimit);

    if (not show_solver){
        setIntegerOption("print_level", 0);
        setIntegerOption("bonmin.bb_log_level", 0);
        setIntegerOption("bonmin.nlp_log_level", 0);
        setStringOption("sb", "yes");
    }

    try {
        impl->Bapp->initialize(GetRawPtr(impl->bonmin_callback));
        Bonmin::Bab bb;
        bb(impl->Bapp);
    }
    catch(Bonmin::TNLPSolver::UnsolvedError *E) {
        solution.setStatus(Solution::SolverStatus::UNSOLVED_ERROR);
        delete E;
    }
    catch(Bonmin::TNLPSolver::UnsolvedError &E) {
        solution.setStatus(Solution::SolverStatus::UNSOLVED_ERROR);
    }
    // Other possible exceptions we may want to catch here:
    // OsiTMINLPInterface::SimpleError &E
    // CoinError &E

    model_changed = false;
}

void BonminModel::setStringOption(std::string key, std::string value){
    impl->Bapp->options()->SetStringValue(key, value);
}

void BonminModel::setNumericOption(std::string key, double value){
    impl->Bapp->options()->SetNumericValue(key, value);
}

void BonminModel::setIntegerOption(std::string key, int value){
    impl->Bapp->options()->SetIntegerValue(key, value);
}
