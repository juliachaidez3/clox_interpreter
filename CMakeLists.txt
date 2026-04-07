cmake_minimum_required(VERSION 3.20)
project(clox_interpreter C)

set(CMAKE_C_STANDARD 99)

add_executable(clox_interpreter
        main.c
        chunk.c
        memory.c
        debug.c
        value.c
        vm.c
        compiler.c
        scanner.c
        object.c
)