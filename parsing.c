#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>

#include "mpc.h"

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

    /* Echo input back to user */
    printf("You have excecuted %s\n", input);

    /* Free retrieved input */
    free(input);
  }

  puts("See you soon");

  mpc_cleanup(4, Number, Operator, Expr, Lispy);
  return 0;
}
