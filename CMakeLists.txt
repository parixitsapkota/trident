cmake_minimum_required(VERSION 4.2)
project(trident C)

set(CMAKE_C_STANDARD 99)

add_executable(trident
  src/main.c
  src/utils/utils.c
  src/utils/utils.h
  src/token/token.c
  src/token/token.h
  src/lexer/lexer.c
  src/lexer/lexer.h
)

target_compile_options(trident PRIVATE -Wall -Wextra)

