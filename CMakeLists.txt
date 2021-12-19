cmake_minimum_required(VERSION 3.17)
project(GrammarParsers)

set(CMAKE_CXX_STANDARD 17)

add_executable(GrammarParsers main.cpp
        Parsers/Earley/EarleyParser.cpp Parsers/Earley/EarleyParser.h
        Grammar.cpp Grammar.h
        Parsers/Parser.cpp Parsers/Parser.h)

target_include_directories(GrammarParsers PRIVATE Parsers Parsers/Earley)