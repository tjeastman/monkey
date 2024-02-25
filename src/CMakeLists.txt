include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(
  main
  main.c
  string.c
  token.c
  lexer.c
  parser.c
  operation.c
  expression.c
  statement.c
  program.c
)

set_property(TARGET main PROPERTY C_STANDARD 17)
