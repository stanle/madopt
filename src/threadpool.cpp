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

#include <stdlib.h>
#include <algorithm>

#include "threadpool.hpp"

#include "inner_constraint.hpp"
#include "adstack.hpp"

namespace MadOpt {

ThreadPool::ThreadPool(InnerConstraint* obj, 
        InnerConstraint** cons,
        size_t size,
        ADStack& ref_stack):
    constraints(cons),
    obj(obj),
    size(size),
    stack(ref_stack),
    finished_threads(size)
    {
        //unsigned int cores = std::thread::hardware_concurrency() - 1;
        //const unsigned int one = 1;
        //cores = std::max(one, cores);

        const unsigned int cores = 1;

        ldiv_t d = ldiv(size, cores+1);

        unsigned int rest = d.rem;

        for (size_t i=0; i<cores; i++){
            unsigned int steps = d.quot;
            if (rest > 0){
                steps += 1;
                rest--;
            }

            workers.emplace_back(&ThreadPool::thread_function,
                    this, 
                    std::ref(ref_stack), 
                    start_pos,
                    steps);
            start_pos += steps;
        }

        count = d.quot + rest;
    }

ThreadPool::~ThreadPool(){
    {
        std::unique_lock<std::mutex> _lock(lock);
        stop = true;
        finished_threads = 0;
    }
    thread_wait.notify_all();
    for (auto&& t : workers)
        t.join();
}

void ThreadPool::setEvals(const double* x){
    xx = x;
    finished_threads = 0;
    thread_wait.notify_all();

    obj->setEvals(x, &stack);

    for (size_t i=start_pos; i<count+start_pos; i++)
        constraints[i]->setEvals(xx, &stack);

    std::unique_lock<std::mutex> _lock(lock);
    if (finished_threads < workers.size())
        main_wait.wait(_lock, 
                [this]{ return finished_threads == workers.size();});
    ASSERT(finished_threads == workers.size());
}

void ThreadPool::thread_function(ADStack& ref_stack, size_t start_pos, size_t count){
    ADStack stack(ref_stack);

    while(1){
        {
            std::unique_lock<std::mutex> _lock(lock);
            thread_wait.wait(_lock, 
                    [this]{ return finished_threads < workers.size();});
        }

        if (stop)
            break;

        ASSERT(finished_threads < workers.size(), finished_threads, size);
        ASSERT(xx != nullptr, finished_threads, size, workers.size());

        for (size_t i=start_pos; i<start_pos+count; i++)
            constraints[i]->setEvals(xx, &stack);

        { 
            std::unique_lock<std::mutex> _lock(lock);
            finished_threads++;
            if (finished_threads == workers.size()){
                main_wait.notify_one();
            }
        }
    }
}


}
