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

#include "threadpool.hpp"

#include "inner_constraint.hpp"
#include "adstack.hpp"

namespace MadOpt {

ThreadPool::ThreadPool(InnerConstraint** cons, size_t size, ADStack& ref_stack):
    constraints(cons),
    size(size)
    {
        unsigned cores = std::thread::hardware_concurrency() - 2;
        current_pos = size;
        for (size_t i=0; i<cores; i++)
            workers.emplace_back(&ThreadPool::thread_function, this, std::ref(ref_stack));
    }

ThreadPool::~ThreadPool(){
    {
        std::unique_lock<std::mutex> _waiting_lock(waiting_lock);
        stop = true;
        current_pos = 0;
        thread_wait.notify_all();
    }
    for (auto&& t : workers)
        t.join();
}

void ThreadPool::setEvals(const double* x){
    xx = x;
    current_pos = 0;
    thread_wait.notify_all();
    std::unique_lock<std::mutex> _waiting_lock(waiting_lock);
    main_wait.wait(_waiting_lock, [this]{ return current_pos == size;});
}

void ThreadPool::thread_function(ADStack& ref_stack){
    InnerConstraint* work;
    ADStack stack(ref_stack);

    while(1){
        {
            std::unique_lock<std::mutex> _waiting_lock(waiting_lock);
            thread_wait.wait(_waiting_lock, [this]{ return current_pos < size;});
            ASSERT(current_pos < size, current_pos, size);
        }

        if (stop)
            break;

        ASSERT(xx != nullptr);

        while(1){
            {
                std::unique_lock<std::mutex> _lock(lock);
                if (current_pos == size){
                    main_wait.notify_one();
                    break;
                }
                work = constraints[current_pos++];
            }
            work->setEvals(xx, &stack);
        }
    }
}


}
