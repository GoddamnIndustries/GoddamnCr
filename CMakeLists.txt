cmake_minimum_required(VERSION 3.5)
project(GoddamnCr)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES
    "Cr Compiler/CrCompiler.cpp"
    "Cr Compiler/Lexeme.cpp"
    "Cr Compiler/Lexeme.h"
    "Cr Compiler/Parser.h"
    "Cr Compiler/Parser.cpp"
    "Cr Compiler/Profile.h"
    "Cr Compiler/Profile.cpp"
    "Cr Compiler/Preprocessor.cpp"
    "Cr Compiler/Preprocessor.h"
    "Cr Compiler/Scanner.cpp"
    "Cr Compiler/Scanner.h"
    "Cr Compiler/Utils.h" "Cr Compiler/AST.cpp" "Cr Compiler/AST.h")

add_executable(GoddamnCr ${SOURCE_FILES})