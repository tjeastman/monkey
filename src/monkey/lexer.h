#ifndef MONKEY_LEXER_H_
#define MONKEY_LEXER_H_

#include <stdbool.h>
#include <stdio.h>

#include "monkey/token.h"

bool lexer_next(FILE*, Token*);

#endif // MONKEY_LEXER_H_
