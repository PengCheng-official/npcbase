cmake_minimum_required(VERSION 3.28)
project(NpcBase)

set(CMAKE_CXX_STANDARD 17)

# Try to locate antlr4-runtime (installed via vcpkg/Conan or local)
find_package(antlr4-runtime CONFIG QUIET)

# If generated parser sources exist, include them
set(ANTLR_GENERATED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated)
if (EXISTS ${ANTLR_GENERATED_DIR})
    message(STATUS "Using ANTLR generated sources from ${ANTLR_GENERATED_DIR}")
    include_directories(${ANTLR_GENERATED_DIR})
    file(GLOB ANTLR_GEN
         ${ANTLR_GENERATED_DIR}/*.cpp
         ${ANTLR_GENERATED_DIR}/*.h)
else()
    message(WARNING "ANTLR generated sources not found. Please generate from grammar/*.g4 into 'generated' directory.")
    set(ANTLR_GEN)
endif()

add_executable(NpcBase src/main.cpp
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

# Link antlr4-runtime if found
if (TARGET antlr4-runtime)
    target_link_libraries(NpcBase PRIVATE antlr4-runtime)
else()
    message(WARNING "antlr4-runtime not found. SQL parser will fall back to simple parsing or fail to build if referencing ANTLR classes.")
endif()
