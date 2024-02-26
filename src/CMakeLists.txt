include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(
  main
  main.c
  token.c
  lexer.c
)

set_property(TARGET main PROPERTY C_STANDARD 17)
