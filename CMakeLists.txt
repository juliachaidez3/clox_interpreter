cmake_minimum_required(VERSION 3.20)
project(clox_interpreter C)

set(CMAKE_C_STANDARD 99)

add_executable(clox_interpreter
        main.c
        memory.c
        value.c
        chunk.c
        debug.c
        scanner.c
        compiler.c
        vm.c
)