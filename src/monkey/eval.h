#ifndef MONKEY_EVAL_H_
#define MONKEY_EVAL_H_

#include "monkey/environment.h"
#include "monkey/object.h"
#include "monkey/statement.h"

bool evaluate_expression(Environment*, Expression*, Object*);
bool evaluate_statement(Environment*, Statement*, Object*);
void evaluate_program(StatementBlock*);

#endif // MONKEY_EVAL_H_
