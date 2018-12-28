#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>

int main(int argc, char **argv) {
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

  return 0;
}

