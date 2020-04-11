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
#ifndef MADOPT_LOGGER_H
#define MADOPT_LOGGER_H

#include <string.h>
#include <iostream>
#include <type_traits>

namespace MadOpt {
 
//#define ENABLE_TRACING
//#define ENABLE_VALGRIND_DEBUGGING
//#define ENABLE_ASSERTS

template<typename TF>
void write_debug_output( std::ostream & out, TF const& f ) {
    out << f;
}
 
struct tracer {
    std::ostream & out;
    tracer( std::ostream & out, char const * file, int line, char const * function )
    : out( out ) {
        out << "[" << file << "][" << line << "][" << function << "] :: ";
    }

    ~tracer() {
        out << std::endl;
    }
 
    template<typename TF, typename ... TR>
    void write( TF const& f, TR const& ... rest ) {
        write_debug_output( out, f );
        out << " ";
        write( rest... );
    }

    template<typename TF>
    void write( TF const& f ) {
        write_debug_output( out, f );
    }

    void write() {}
};

#define THIS_FILE (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define PRINT_STUFF_OUT(...) tracer( std::cout, THIS_FILE, __LINE__, __func__ ).write( __VA_ARGS__ )

// Logger stuff
//
//
#ifdef ENABLE_TRACING
#define TRACE(...) PRINT_STUFF_OUT(__VA_ARGS__)
#else
#define TRACE(...) 
#endif

#define TRACE_END TRACE("End")
#define TRACE_START TRACE("Start")

// debug stuff
//
//

#ifdef ENABLE_VALGRIND_DEBUGGING
#define VALGRIND_CONDITIONAL_JUMP_TEST(data) if (data == 13.12435) std::cout<<"Valgrind conditional jump test"<<std::endl

#define VALGRIND_CONDITIONAL_JUMP_TEST_LOOP(cn, data) for (size_t i=0; i<(size_t)cn; i++) VALGRIND_CONDITIONAL_JUMP_TEST(data[i]) 

#else
#define VALGRIND_CONDITIONAL_JUMP_TEST(data)
#define VALGRIND_CONDITIONAL_JUMP_TEST_LOOP(cn, data)
#endif

// assert
//
//

#ifdef ENABLE_ASSERTS

#define ASSERT_ENABLED true

#define ASSERT(condition,...) do { \
        if (condition) {} else {\
            PRINT_STUFF_OUT("*** ASSERT FAILED ***");\
            PRINT_STUFF_OUT(#condition);\
            PRINT_STUFF_OUT(__VA_ARGS__);\
            abort();\
        }\
    } while(0)

#define ASSERT_EQ(a,b,...) do { \
        if (a==b) {} else {\
            PRINT_STUFF_OUT("*** ASSERT FAILED ***");\
            PRINT_STUFF_OUT(#a, "!=", #b);\
            PRINT_STUFF_OUT(a, "!=", b);\
            PRINT_STUFF_OUT(__VA_ARGS__);\
            abort();\
        }\
    } while(0)

#define ASSERT_LE(a,b,...) do { \
        if (a<=b) {} else {\
            PRINT_STUFF_OUT("*** ASSERT FAILED ***");\
            PRINT_STUFF_OUT(#a, "!<=", #b);\
            PRINT_STUFF_OUT(a, "!<=", b);\
            PRINT_STUFF_OUT(__VA_ARGS__);\
            abort();\
        }\
    } while(0)

#define ASSERT_BETWEEN(a,b,c,...) do { \
        if (a<=b) {} else {\
            PRINT_STUFF_OUT("*** ASSERT FAILED ***");\
            PRINT_STUFF_OUT(#a, "!<=", #b);\
            PRINT_STUFF_OUT(a, "!<=", b);\
            PRINT_STUFF_OUT(__VA_ARGS__);\
            abort();\
        }\
        if (b<=c) {} else{\
            PRINT_STUFF_OUT("*** ASSERT FAILED ***");\
            PRINT_STUFF_OUT(#b, "!<=", #c);\
            PRINT_STUFF_OUT(b, "!<=", c);\
            PRINT_STUFF_OUT(__VA_ARGS__);\
            abort();\
        }\
    } while(0)

#define ASSERT_UEQ(a,b,...) do { \
        if (a!=b) {} else {\
            PRINT_STUFF_OUT("*** ASSERT FAILED ***");\
            PRINT_STUFF_OUT(#a, "==", #b);\
            PRINT_STUFF_OUT(a, "==", b);\
            PRINT_STUFF_OUT(__VA_ARGS__);\
            abort();\
        }\
    } while(0)

#define ASSERT_IF(a,b,...) do { \
        if (!(a) || (b)) {} else {\
            PRINT_STUFF_OUT("*** ASSERT FAILED ***");\
            PRINT_STUFF_OUT("NOT IF ", #a, " THEN ", #b);\
            PRINT_STUFF_OUT(a, " THEN ", b);\
            PRINT_STUFF_OUT(__VA_ARGS__);\
            abort();\
        }\
    } while(0)

#define ASSERT_XOR(a,b,...) do { \
        if (((a) && (b)) || (!(a) && !(b))) {} else {\
            PRINT_STUFF_OUT("*** ASSERT FAILED ***");\
            PRINT_STUFF_OUT(#a, " XOR ", #b);\
            PRINT_STUFF_OUT(a, " XOR ", b);\
            PRINT_STUFF_OUT(__VA_ARGS__);\
            abort();\
        }\
    } while(0)

#define ASSERT_UNIQUEELEMENTS(a,...) do { \
	if (number_of_unique_elements(a) != a.size()) {\
            PRINT_STUFF_OUT("*** ASSERT FAILED ***");\
            PRINT_STUFF_OUT(#a, " does not have unique elements");\
            PRINT_STUFF_OUT(to_string(a));\
            PRINT_STUFF_OUT(__VA_ARGS__);\
            abort();\
	} \
    } while(0)

#define DEBUG_CODE(a) a

#else
#define ASSERT_ENABLED false
#define ASSERT(...)
#define ASSERT_EQ(...)
#define ASSERT_LE(...)
#define ASSERT_UEQ(...)
#define ASSERT_IF(...)
#define ASSERT_XOR(...)
#define ASSERT_BETWEEN(...)
#define ASSERT_UNIQUEELEMENTS(...)
#define DEBUG_CODE(a)
#endif

}

#endif
