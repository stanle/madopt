cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0015 NEW)
project(MADOPT)
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   3   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   1   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

INCLUDE (CheckCXXSourceCompiles)

set(CXX_FLAGS_COM "${CXX_FLAGS_COM} -std=c++11 -fPIC")

# cmake automatically sets some flags based on compiler for debug and release
# Can select which we want with -DCMAKE_BUILD_TYPE=release
set(CMAKE_CXX_FLAGS "${CXX_FLAGS_COM} ${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CXX_FLAGS_COM} ${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CXX_FLAGS_COM} ${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_PROFILE "${CXX_FLAGS_COM} ${CMAKE_CXX_FLAGS_PROFILE} -g -pg")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

if (CUSTOM_INCLUDE)
    include_directories( ${CUSTOM_INCLUDE} )
endif()

if (CUSTOM_LIBRARY)
    link_directories( ${CUSTOM_LIBRARY} )
endif()

set(TEST_DIR tests)

set(SRC_DIR src)

include_directories(
    ${SRC_DIR}/
	)

add_library(madopt STATIC
    ${SRC_DIR}/model.cpp
    ${SRC_DIR}/constraint.cpp
    ${SRC_DIR}/common.cpp
    ${SRC_DIR}/expr.cpp
    ${SRC_DIR}/inner_var.cpp
    ${SRC_DIR}/inner_constraint.cpp
    ${SRC_DIR}/solution.cpp
    ${SRC_DIR}/var.cpp
    ${SRC_DIR}/cstack.cpp
    ${SRC_DIR}/simstack.cpp
    ${SRC_DIR}/pairhashmap.cpp
	)

install(TARGETS madopt ARCHIVE DESTINATION lib)

# IPOPT 
#
#
#
add_library(madopt_ipopt STATIC
    ${SRC_DIR}/ipopt_nlp.cpp
    ${SRC_DIR}/ipopt_model.cpp
    )

install(TARGETS madopt_ipopt ARCHIVE DESTINATION lib)

install(DIRECTORY src/ DESTINATION include/madopt FILES_MATCHING PATTERN "*.hpp")

# DOC
#
#
#
find_package(Doxygen)
option(BUILD_DOC "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})
option(BUILD_FULL_DOC "Create the full documentation, otherwise only the
important methods and classes for the solver front end are documented" OFF)

if (BUILD_FULL_DOC)
    set (BUILD_FULL_DOC_TMP "NO")
else()
    set (BUILD_FULL_DOC_TMP "YES")
endif()

if(BUILD_DOC)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)

    #    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION
    #        share/doc/${CMAKE_PROJECT_NAME})
endif()

# Bonmin
#
#
#
find_library(BONMIN_LIB NAMES bonmin)

if (BONMIN_LIB)
    set(CMAKE_REQUIRED_LIBRARIES ${BONMIN_LIB})

    #check_cxx_source_compiles

    CHECK_CXX_SOURCE_COMPILES("
        #include <coin/BonTMINLP.hpp> 
        int main(void) {
            Bonmin::TMINLP::SolverReturn::USER_INTERRUPT;
        }"
        BONMIN_HAS_USER_INTERRUPT)

    if (NOT BONMIN_HAS_USER_INTERRUPT)
        set(BONMIN_HAS_USER_INTERRUPT "-D BONMIN_HAS_USER_INTERRUPT")
        set(CMAKE_CXX_FLAGS 
            "${CXX_FLAGS_COM} ${CMAKE_CXX_FLAGS} ${BONMIN_HAS_USER_INTERRUPT}")
        set(CMAKE_CXX_FLAGS_DEBUG 
            "${CXX_FLAGS_COM} ${CMAKE_CXX_FLAGS_DEBUG} ${BONMIN_HAS_USER_INTERRUPT}")
        set(CMAKE_CXX_FLAGS_RELEASE 
            "${CXX_FLAGS_COM} ${CMAKE_CXX_FLAGS_RELEASE} ${BONMIN_HAS_USER_INTERRUPT}")
        set(CMAKE_CXX_FLAGS_PROFILE 
            "${CXX_FLAGS_COM} ${CMAKE_CXX_FLAGS_PROFILE} ${BONMIN_HAS_USER_INTERRUPT}")
    endif()

    add_library(madopt_bonmin STATIC
        ${SRC_DIR}/bonmin_minlp.cpp
        ${SRC_DIR}/bonmin_model.cpp
        )

    install(TARGETS madopt_bonmin ARCHIVE DESTINATION lib)
endif()

# Minitest
#
#
#
if (MINITEST)
    add_executable(minitest
        ${TEST_DIR}/minitest.cpp
    )

    target_link_libraries(minitest 
        madopt 
        madopt_ipopt
        madopt_bonmin
        bonmin
        ipopt
        ) 

endif()

# Test
#
#
#
find_package(CxxTest)
if(CXXTEST_FOUND)
    include_directories(${CXXTEST_INCLUDE_DIR})
    enable_testing()

    if (BONMIN_LIB)
        set(BONMIN_TEST "../${TEST_DIR}/bonminmodel_tests.hpp")
        set(BONMIN_MADOPT_LIB "madopt_bonmin")
    else()
        set(BONMIN_TEST " ")
        set(BONMIN_MADOPT_LIB " ")
    endif()

    CXXTEST_ADD_TEST(unittest_madopt unittest_madopt.cpp
        ../${TEST_DIR}/expr_tests.hpp
        ../${TEST_DIR}/econstraint_tests.hpp
        ../${TEST_DIR}/model_tests.hpp
        ../${TEST_DIR}/ipoptmodel_tests.hpp
        ${BONMIN_TEST}
    )

    target_link_libraries(unittest_madopt 
        ${BONMIN_MADOPT_LIB}
        ${BONMIN_LIB}
        madopt
        madopt_ipopt
        ipopt
        ) 
endif()

# ex: set tabstop=4 shiftwidth=4 expandtab:
