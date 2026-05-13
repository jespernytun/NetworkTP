#include "BAL-mgmt.h"

// ADD_LETTER FUNCTION
void add_letter(bal_head* BAL, int box_ID_dest, char* msg, int lg_msg) {
  
  /* making an auxilliary*/
  boite_lettre* aux = BAL->pfirstbox;
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
  if (aux == NULL) {
    boite_lettre* new_box = malloc(sizeof(boite_lettre));
    new_box->box_ID = box_ID_dest;
    new_box->nb_msg = 0;
    new_box->pfirstmsg = NULL;
    new_box->pnextbox = NULL;

    if (prev == NULL) BAL->pfirstbox = new_box;
    else prev->pnextbox = new_box;

    BAL->nb_boites++;
    aux = new_box;
  }

  // ADDING THE MESSAGE TO THE BOX
  une_msg* new_msg = malloc(sizeof(une_msg));
  strncpy(new_msg->msg, msg, LG_MAX_MSG);
  new_msg->msg[LG_MAX_MSG - 1] = '\0'; 
  new_msg->lg_msg = lg_msg;            
  new_msg->pnextmsg = NULL;

  
  if (aux->pfirstmsg == NULL) {
    aux->pfirstmsg = new_msg;
  } else {
    une_msg* last = aux->pfirstmsg;
    while (last->pnextmsg != NULL) last = last->pnextmsg;
    last->pnextmsg = new_msg;
  }

  aux->nb_msg++;
}

// AFFICHE_LETTER FUNCTION
// Sends all letters of a box over the socket, then frees the messages

void affiche_letter(bal_head* BAL, int box_ID_dest, int sock) {
  
  boite_lettre* aux = BAL->pfirstbox;

  /* searching box that we want to print */
  while (aux != NULL) {
    if (aux->box_ID == box_ID_dest) break;
    aux = aux->pnextbox;
  }

  if (aux == NULL) {
    printf("Sorry, no new messages for box %d\n", box_ID_dest);
    return;
  }

  /* Send each message then free it */
  une_msg* auxmsg = aux->pfirstmsg;
  while (auxmsg != NULL) {
    afficher_message(auxmsg->msg, auxmsg->lg_msg);

    if (send(sock, auxmsg->msg, auxmsg->lg_msg, 0) < 0) {
      perror("send");
    }

    une_msg* tmp = auxmsg;
    auxmsg = auxmsg->pnextmsg;
    free(tmp); 
  }


  aux->pfirstmsg = NULL;
  aux->nb_msg = 0;

  
}
