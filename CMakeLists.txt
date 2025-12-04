cmake_minimum_required(VERSION 3.28)
project(NpcBase)

set(CMAKE_CXX_STANDARD 17)

# ------------------------------------------------------------
# Use ANTLR4 C++ runtime from the project folder
# ------------------------------------------------------------
# We assume you have checked in ANTLR4 sources under:
#   ${CMAKE_SOURCE_DIR}/antlr4/runtime/Cpp
# This will build the runtime locally and expose targets like
#   antlr4_shared / antlr4_static / antlr4-runtime / antlr4_runtime
# ------------------------------------------------------------

# Disable building/running tests in the ANTLR runtime to avoid gtest discovery issues
set(BUILD_TESTING OFF CACHE BOOL "Disable GoogleTest in subprojects" FORCE)
# Prefer static libraries to avoid missing DLLs on end-user runtime
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Prefer static linkage" FORCE)

set(LOCAL_ANTLR_RUNTIME_DIR "${CMAKE_SOURCE_DIR}/antlr4/runtime/Cpp")
if (EXISTS ${LOCAL_ANTLR_RUNTIME_DIR}/CMakeLists.txt)
    message(STATUS "Using in-project ANTLR4 runtime at ${LOCAL_ANTLR_RUNTIME_DIR}")
    # Avoid building tools; we only need the C++ runtime
    set(ANTLR4_WITH_TOOLS OFF CACHE BOOL "" FORCE)
    set(ANTLR4_INSTALL OFF CACHE BOOL "" FORCE)
    add_subdirectory(${LOCAL_ANTLR_RUNTIME_DIR} ${CMAKE_BINARY_DIR}/antlr4-runtime)
else()
    message(FATAL_ERROR "ANTLR4 runtime not found in project at ${LOCAL_ANTLR_RUNTIME_DIR}. Please add antlr4/runtime/Cpp.")
endif()

# ------------------------------------------------------------
# Generated parser/lexer sources
# ------------------------------------------------------------
set(ANTLR_GENERATED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated)
if (EXISTS ${ANTLR_GENERATED_DIR})
    message(STATUS "Using ANTLR generated sources from ${ANTLR_GENERATED_DIR}")
    include_directories(${ANTLR_GENERATED_DIR})
    file(GLOB ANTLR_GEN
         ${ANTLR_GENERATED_DIR}/*.cpp
         ${ANTLR_GENERATED_DIR}/*.cc
         ${ANTLR_GENERATED_DIR}/*.cxx
         ${ANTLR_GENERATED_DIR}/*.h
         ${ANTLR_GENERATED_DIR}/*.hpp)
else()
    message(WARNING "ANTLR generated sources not found. Please generate from grammar/*.g4 into 'generated' directory.")
    set(ANTLR_GEN)
endif()

# ------------------------------------------------------------
# Main target
# ------------------------------------------------------------
add_executable(NpcBase
        src/main.cpp
        include/npcbase.h
        include/data_dict.h
        src/data_dict.cpp
        src/mem_manager.cpp
        include/mem_manager.h
        src/disk_manager.cpp
        include/disk_manager.h
        src/table_manager.cpp
        include/table_manager.h
        src/cli.cpp
        include/cli.h
        src/log_manager.cpp
        include/log_manager.h
        src/test.cpp
        include/test.h
        src/index_manager.cpp
        include/index_manager.h
        src/sql_parser.cpp
        include/sql_ast.h
        src/sql_plan.cpp
        include/sql_plan.h
        src/sql_physical.cpp
        include/sql_physical.h
        ${ANTLR_GEN}
)

# Link antlr4 runtime (prefer static)
if (TARGET antlr4_static)
    target_link_libraries(NpcBase PRIVATE antlr4_static)
elseif(TARGET antlr4_runtime)
    target_link_libraries(NpcBase PRIVATE antlr4_runtime)
elseif(TARGET antlr4-runtime)
    target_link_libraries(NpcBase PRIVATE antlr4-runtime)
elseif(TARGET antlr4_shared)
    target_link_libraries(NpcBase PRIVATE antlr4_shared)
else()
    message(FATAL_ERROR "In-project ANTLR4 runtime added, but no linkable target was found.")
endif()
