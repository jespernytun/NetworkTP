#include "tools.h"

/* Function that prints a message to the terminal */
void afficher_message (char *message, int lg) {
  int i;
  printf("message : ");
  for (i=0 ; i<lg ; i++) printf("%c", message[i]);
  printf("\n");
}

void construire_message (char *message, char motif, int lg) {
  int i;
  for (i=0; i<lg; i++) message[i] = motif;
}
