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
#include "ipopt_model.hpp"
#include "ipopt_nlp.hpp"

using namespace MadOpt;

namespace MadOpt {
    struct IpoptModelImpl {
        IpoptModelImpl(IpoptModel* model){
            Iapp.Initialize();
            ipopt_callback = new IpoptUserClass(model);
        }
        IpoptApplication Iapp;
        Ipopt::SmartPtr<IpoptUserClass> ipopt_callback;
    };
}

IpoptModel::IpoptModel(){
    impl = new IpoptModelImpl(this);
}

IpoptModel::~IpoptModel(){
    delete impl;
}

void IpoptModel::solve(){
    if (not show_solver){
        setIntegerOption("print_level", 0);
        setStringOption("sb", "yes");
    }

    if (timelimit >= 0)
        setNumericOption("max_cpu_time", timelimit);

    if (model_changed)
        impl->Iapp.OptimizeTNLP(impl->ipopt_callback);
    else
        impl->Iapp.ReOptimizeTNLP(impl->ipopt_callback);

    model_changed = false;
}

void IpoptModel::setStringOption(std::string key, std::string value){
    impl->Iapp.Options()->SetStringValue(key, value);
}

void IpoptModel::setNumericOption(std::string key, double value){
    impl->Iapp.Options()->SetNumericValue(key, value);
}

void IpoptModel::setIntegerOption(std::string key, int value){
    impl->Iapp.Options()->SetIntegerValue(key, value);
}
