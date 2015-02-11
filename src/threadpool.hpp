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

#ifndef MADOPT_THREADPOOL_H
#define MADOPT_THREADPOOL_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

namespace MadOpt {

class InnerConstraint;
class ADStack;

class ThreadPool {
    public:
        ThreadPool(InnerConstraint*, InnerConstraint**, size_t, ADStack&);
        
        ~ThreadPool();

        void setEvals(const double* x);

    private:
        std::vector<std::thread> workers;

        std::mutex lock;

        std::condition_variable thread_wait;

        std::condition_variable main_wait;

        InnerConstraint** constraints;

        InnerConstraint* obj;

        size_t size;

        const double* xx = nullptr;

        bool stop = false;

        ADStack& stack;

        size_t finished_threads;

        size_t count;

        size_t start_pos;

        void thread_function(ADStack&, size_t, size_t);
};

}

#endif
/* ex: set tabstop=4 shiftwidth=4 expandtab: */
