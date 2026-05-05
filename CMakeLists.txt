cmake_minimum_required(VERSION 3.10)
project(MVCC_DB)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(mvcc-db
    src/main.cpp
    src/engine/engine.cpp
    src/transaction/transaction.cpp
    src/storage/table.cpp
    src/utils/logger.cpp
)
