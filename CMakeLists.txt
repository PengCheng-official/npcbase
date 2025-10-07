cmake_minimum_required(VERSION 3.28)
project(NpcBase)

set(CMAKE_CXX_STANDARD 17)

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
)
