#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>

#include "mpc.h"

enum { LVAL_NUM, LVAL_ERR };
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

typedef struct {
  int type;
  long num;
  int err;
} lval;

lval lval_num(long x);
lval lval_err(int x);
void lval_print(lval v);
void lval_println(lval v);

lval eval(mpc_ast_t* t);
lval eval_op(lval x, char* op, lval y);

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
      lval result = eval(r.output);
      lval_println(result);
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


lval lval_num(long x) {
  lval v;
  v.type = LVAL_NUM;
  v.num = x;

  return v;
}

lval lval_err(int x) {
  lval v;
  v.type = LVAL_ERR;
  v.err = x;

  return v;
}

void lval_print(lval v) {
  switch (v.type) {
    case LVAL_NUM:
      printf("%li", v.num);
      break;
    case LVAL_ERR:
      if(v.err == LERR_DIV_ZERO) {
        printf("Error: Division By Zero!");
      }
      if(v.err == LERR_BAD_OP) {
        printf("Error: Invalid Operator!");
      }
      if(v.err == LERR_BAD_NUM) {
        printf("Error: Invalid Number!");
      }
      break;
  }
}


void lval_println(lval v) {
  lval_print(v);
  putchar('\n');
}

lval eval(mpc_ast_t* t) {
  /* If tagged as number return it directly */
  if(strstr(t->tag, "number")) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE? lval_num(x) : lval_err(LERR_BAD_NUM);
  }

  /* The Operator is always second child */
  char* op = t->children[1]->contents;

  /* We store the third child in 'x' */
  lval x = eval(t->children[2]);

  /* Iterate the remaining children and combining */
  int i = 3;
  while(strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  return x;
}


lval eval_op(lval x, char* op, lval y) {
  if(x.type == LVAL_ERR) { return x; }
  if(y.type == LVAL_ERR) { return y; }

  if(strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
  if(strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
  if(strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
  if(strcmp(op, "/") == 0) { return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num); }

  return lval_err(LERR_BAD_OP);
}
