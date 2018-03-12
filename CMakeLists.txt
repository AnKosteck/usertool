cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project(usertool CXX)

set(SRCDIR src)
set(INCDIR include)

set(usertoolSRC
    ${SRCDIR}/bsd2linux.cc
    ${SRCDIR}/daemon.cc
    ${SRCDIR}/database.cc
    ${SRCDIR}/filtersystem.cc
    ${SRCDIR}/flawsearch.cc
    ${SRCDIR}/group.cc
    ${SRCDIR}/linux2bsd.cc
    ${SRCDIR}/passwordupdate.cc
    ${SRCDIR}/removeusers.cc
    ${SRCDIR}/string_methods.cc
    ${SRCDIR}/system.cc
    ${SRCDIR}/system2yaml.cc
    ${SRCDIR}/systemupdate.cc
    ${SRCDIR}/updatedatabase.cc
    ${SRCDIR}/user.cc
    ${SRCDIR}/usertool.cc)


set(usertoolINCLUDE
    ${INCDIR}/database.h
    ${INCDIR}/group.h
    ${INCDIR}/string_methods.h
    ${INCDIR}/system.h
    ${INCDIR}/user.h
    ${INCDIR}/usertool.h)
    
add_executable(usertool ${usertoolSRC})

set(cmake_cxx_extensions off)
set(needed_features
    cxx_range_for
    cxx_auto_type)
SET( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wall )

find_package(yaml-cpp 0.5.2 REQUIRED)

#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

include_directories(${YAML_CPP_INCLUDE_DIR} ${INCDIR})
target_link_libraries(usertool ${LIBS} ${YAML_CPP_LIBRARIES})
target_compile_features(usertool PRIVATE ${needed_features})
