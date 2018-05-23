cmake_minimum_required(VERSION 3.7.1)
project(execasm C ASM)

set(CMAKE_C_STANDARD 11)

add_definitions("-fno-pic -fno-pie -Wall -Wa,-msyntax=intel -masm=intel --save-temps")

set(CMAKE_ASM_FLAGS "${CFLAGS} -x assembler-with-cpp")

add_executable(execasm main.c asmfuck.S asmfuck.h)
