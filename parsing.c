#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>

#include "mpc.h"

long eval(mpc_ast_t* t);
long eval_op(long x, char* op, long y);

int main(int argc, char **argv) {
  /* Create Some Parsers */
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Lispy = mpc_new("lispy");

  /* Define them whit the following Language */
  mpca_lang(MPCA_LANG_DEFAULT,
      "\
      number: /-?[0-9]+/ ; \
      operator: '+' | '-' | '*' | '/' ; \
      expr: <number> | '(' <operator> <expr>+ ')' ; \
      lispy: /^/ <operator> <expr>+ /$/ ; \
      ",
      Number, Operator, Expr, Lispy);

  /* Print Version and Exit Information */
  puts("Lispy Version 0.0.1");
  puts("Press Ctrl+c to Exit \n");

  /* In a never ending Loop */
  while(1) {
    /* Output our prompt and get input */
    char* input = readline("JuanCrg90> ");

    /* Add input to history */
    add_history(input);

    /* Attempt to Parse the user input */
    mpc_result_t r;
    if(mpc_parse("<stdin>", input, Lispy, &r)) {
      long result = eval(r.output);
      printf("%li\n", result);
      mpc_ast_delete(r.output);
    } else {
      /* Otherwise Print the Error */
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    /* Free retrieved input */
    free(input);
  }

  puts("See you soon");

  mpc_cleanup(4, Number, Operator, Expr, Lispy);
  return 0;
}


long eval(mpc_ast_t* t) {
  /* If tagged as number return it directly */
  if(strstr(t->tag, "number")) {
    return atoi(t-> contents);
  }

  /* The Operator is always second child */
  char *op = t->children[1]->contents;

  /* We store the third child in 'x' */
  long x = eval(t->children[2]);

  /* Iterate the remaining children and combining */
  int i = 3;
  while(strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  return x;
}


long eval_op(long x, char* op, long y) {
  if(strcmp(op, "+") == 0) { return x + y; }
  if(strcmp(op, "-") == 0) { return x - y; }
  if(strcmp(op, "*") == 0) { return x * y; }
  if(strcmp(op, "/") == 0) { return x / y; }
  return 0;
}
