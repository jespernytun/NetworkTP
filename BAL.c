// LINKED LISTS FOR BAL LETTER MANAGEMENT

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define LG_MAX_MSG 100

/* linked list data type*/
typedef struct bal_head {
  int nb_boites;
  struct boite_lettre* pfirstbox;
} bal_head;

typedef struct boite_lettre {
  int box_ID;
  int nb_msg;
  struct une_msg* pfirstmsg;     /* POINTS TOWARDS THE MESSAGES   */
  struct boite_lettre* pnextbox; /* POINTS TOWARDS NEXT LETTERBOX */
} boite_lettre;

typedef struct une_msg {
  char * msg[LG_MAX_MSG];
  int lg_msg;
  struct une_msg* pnextmsg;   /* NEXT MESSAGE */
} une_msg;

/* Function that prints a message to the terminal */
void afficher_message (char *message, int lg) {
  int i;
  printf("message construit : ");
  for (i=0 ; i<lg ; i++) printf("%c", message[i]) ; printf("\n");
}


// ADD_LETTER FUCNTION
// This function searches for the box with a user defined ID
// If box doesn't yet exist, it will be created
// Adds letter to the top of the letterbox
void add_letter(bal_head BAL, int box_ID_dest, char* msg) {
  
  /* making an auxilliary*/
  boite_lettre* aux = BAL.pfirstbox;
  boite_lettre* prev = NULL;

  while (aux != NULL) {
    if (aux->box_ID == box_ID_dest){
      break;
    } // BOX FOUND
      // else
    prev = aux;
    aux = aux->pnextbox;
  }

  // if box not found we need to create a box
  // if aux = NULL, other boxes exist and we add it to the last box
  if (aux == NULL) {
    boite_lettre* new_box = malloc(sizeof(boite_lettre));
    new_box->box_ID = box_ID_dest;
    new_box->nb_msg = 0;
    new_box->pfirstmsg = NULL;
    new_box->pnextbox = NULL;

    // if prev is null, it means no boxes have been made yet
    // we add it to the head
    if (prev == NULL) BAL.pfirstbox = new_box; // first boite a lettre
    else prev->pnextbox = new_box;

    // we add to the box counter
    BAL.nb_boites++;
    aux = new_box; // and we add the new_box to the end of the chain
  }

  // the box has successfully created or found
  // ADDING THE MESSAGE TO THE BOX
  une_msg* new_msg = malloc(sizeof(une_msg));
  strncpy(new_msg->msg, msg, LG_MAX_MSG);
  new_msg->lg_msg = strlen(msg); // adding message lenght
  new_msg->pnextmsg = aux->pfirstmsg; // we make our message point to the chain of messages in bal
  aux->pfirstmsg = new_msg; // and then make our BAL point to our message
  aux->nb_msg++; // add to message counter
    
}

// READ_LETTER FUNCTION
// This function prints all letter of an associated box then deletes it
void affiche_letter(bal_head BAL, int box_ID_dest, char* msg) {
  
  /* making an auxilliary*/
  boite_lettre* aux = BAL.pfirstbox;
  boite_lettre* prev = NULL;

  /* searching box that we want to print */
  while (aux != NULL) {
    if (aux->box_ID == box_ID_dest){
      break;
    } // BOX FOUND
      // else
    prev = aux;
    aux = aux->pnextbox;
  }

  if (aux == NULL) {
    /* NO BOX HAS BEEN FOUND */
    printf("Sorry, no new messages\n");
  }

  // creating an auxilliary
  une_msg * auxmsg = aux->pfirstmsg;
  while (auxmsg->pnextmsg!=NULL) {
    afficher_message(auxmsg->msg, auxmsg->lg_msg);
    auxmsg = auxmsg->pnextmsg;
  }
  
  /* DELETE BOITE A LETTRES*/
  prev = aux->pnextbox;
    
}
